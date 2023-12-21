import gradio as gr
import vertexai
from vertexai.language_models import TextGenerationModel, TextEmbeddingModel
import numpy as np
from scipy.spatial import distance
import nltk
import gradio as gr
from youtube_transcript_api import YouTubeTranscriptApi
import vertexai
from google.cloud import texttospeech
import base64
from googleapiclient.discovery import build
import requests
import datetime

vertexai.init(project="erazuthmohandasrakesh-emr", location="us-central1")

# Constants
temperature = 0.2
max_decode_steps = 256
top_p = 0.95
top_k = 40
api_key = XXXXXXX

def predict_large_language_model_sample(model_name, content, temperature, max_decode_steps, top_p, top_k):
    parameters = {
        "temperature": temperature,
        "max_output_tokens": max_decode_steps,
        "top_p": top_p,
        "top_k": top_k
    }
    model = TextGenerationModel.from_pretrained(model_name)
    response = model.predict(content, **parameters)
    return response.text

turn 'The text does not contain an answer to this question.'

def get_transcript(video_id):
    try:
        transcript = YouTubeTranscriptApi.get_transcript(video_id)
        return " ".join([t['text'] for t in transcript]).replace("\n", " ")
    except Exception as e:
        return str(e)



def get_embedding(text):
    if not text.strip():  # Checks if the text is empty or whitespace
        # Return some default value or handle it appropriately
        return None
    embedding_model = TextEmbeddingModel.from_pretrained("textembedding-gecko@001")
    text = remove_stop_words(text)
    return embedding_model.get_embeddings([text])[0].values


def get_answer(video_id, question, model_name, temperature, max_decode_steps, top_p, top_k):
    merged_text = get_transcript(video_id)
    prompt_question = f'''Answer the question: {question}, based on the below context. \n Context: {merged_text} \n '''
    return predict_large_language_model_sample(model_name, prompt_question, temperature, max_decode_steps, top_p, top_k)

def get_summary(video_id, model_name, temperature, max_decode_steps, top_p, top_k):
    merged_text = get_transcript(video_id)
    prompt_summary = "Summarize all the text below in a few sentences.\n" + merged_text
    return predict_large_language_model_sample(model_name, prompt_summary, temperature, max_decode_steps, top_p, top_k)

def get_translate(answer_text, language, model_name, temperature, max_decode_steps, top_p, top_k):
    prompt_translate = f"Translate to {language}:\n" + answer_text
    return predict_large_language_model_sample(model_name,prompt_translate, temperature, max_decode_steps, top_p, top_k)

def jumpto_timestamps(video_id, question, model_name, temperature, max_decode_steps, top_p, top_k):
    merged_text = get_transcript(video_id)
    ans_qna = chunk_search_qna(merged_text, question, model_name, temperature, max_decode_steps, top_p, top_k)
    ans_qna_embeddings = get_embedding(ans_qna)

    transcripts = YouTubeTranscriptApi.get_transcript(video_id)
    embedded_transcript = [get_embedding(t['text']) for t in transcripts if t['text'] and t['text'].strip()]

    
    answers_in_video = []

    for idx, item in enumerate(embedded_transcript):
        if item is None:
            continue
        cos_sim = 1 - distance.cosine(ans_qna_embeddings, item)
        if cos_sim > 0.8:
            answers_in_video.append(str(transcripts[idx]['start']))
    return answers_in_video





def text_to_speech(text, target_language):
    """Converts text to speech."""
    client = texttospeech.TextToSpeechClient()

    # Translate language name to Google's language codes.
    language_mapping = {
        'English': 'en-US',
        'French': 'fr-FR',
        'German': 'de-DE',
        'Hindi': 'hi-IN',
        'Bengali': 'bn-IN',
        'Telugu': 'te-IN',
        'Marathi': 'mr-IN',
        'Tamil': 'ta-IN',
        'Gujarati': 'gu-IN',
        'Kannada': 'kn-IN',
        'Malayalam': 'ml-IN',
        'Oriya': 'or-IN',
        'Punjabi': 'pa-IN',
        'Assamese': 'as-IN'
    }

    language_code = language_mapping.get(target_language, 'en-US')  # default to English if not found

    synthesis_input = texttospeech.SynthesisInput(text=text)
    
    voice = texttospeech.VoiceSelectionParams(
        language_code=language_code,
        ssml_gender=texttospeech.SsmlVoiceGender.NEUTRAL
    )

    audio_config = texttospeech.AudioConfig(
        audio_encoding=texttospeech.AudioEncoding.MP3
    )

    response = client.synthesize_speech(
        input=synthesis_input, voice=voice, audio_config=audio_config
    )

    # The response's audio_content is binary. Write it to an MP3 file.
    audio_file_path = 'rakeshmohandas/youtube_summariser/output.mp3'
    with open(audio_file_path, 'wb') as out:
        out.write(response.audio_content)
    
    return audio_file_path




def get_youtube_stats(video_id, api_key):
    base_url = "https://www.googleapis.com/youtube/v3/videos"
    part = "snippet,statistics"
    
    # Construct the URL
    url = f"{base_url}?id={video_id}&part={part}&key={api_key}"
    
    # Make the API request
    response = requests.get(url)
    
    if response.status_code == 200:
        data = response.json()
        if 'items' in data and len(data['items']) > 0:
            statistics = data['items'][0]['statistics']
            title = data['items'][0]['snippet']['title']
            statistics['title'] = title
            return statistics
    else:
        print(f"Error {response.status_code}: {response.text}")

    return None

    return iframe_html, answer, summary, translation, ", ".join(timestamps), audio_html, stats_data


gr.Interface(
    fn=interface,
    inputs=[
        gr.inputs.Textbox(placeholder="YouTube Video ID",label="Please provide the YouTube Video ID"),
        gr.inputs.Textbox(placeholder="Enter your question",label="Ask any Questions on the Youtube Video here"),
        gr.inputs.Dropdown(choices=["Hindi", "English", "Telugu", "Marathi","Malayalam","Tamil","Telugu"], label="Select Translation Language"),  # Add a dropdown for language selection
        gr.inputs.Dropdown(choices=["text-bison@001", "text-bison-32k","text-bison"], label="Model Choice"),
        gr.inputs.Slider(minimum=0.1, maximum=1, default=0.2, label="Temperature"),
        gr.inputs.Slider(minimum=10, maximum=1024, default=256, step=1, label="Max Tokens"),
        gr.inputs.Slider(minimum=0.1, maximum=1, default=0.95, label="Top P"),
        gr.inputs.Slider(minimum=1, maximum=100, default=40, step=1, label="Top K")
    ],
    outputs=[
        gr.outputs.HTML(label="YouTube Video"),
        gr.outputs.Textbox(label="Answer"),
        gr.outputs.Textbox(label="Summary"),
        gr.outputs.Textbox(label="Translation"),
        gr.outputs.Textbox(label="Timestamps"),
        gr.outputs.HTML(label="Translated Audio")
    ],   
    live=False,
    title="<center><h1>Instant Video Insights: Ask, Translate & Dive Deep into Any YouTube Video!<h1><center>",
    description="<center><i>AI-Powered Deep Dive: Analyze, Answer & Summarize YouTube Videos.</i></center>",
    article="By Rakesh Mohandas, CE-Data Analytics",
    examples=[
        ["XXXXXYYYYYZZ", "whats the video all about?", "Hindi","text-bison@001",0.2,256,0.95,40],
        ["XXXXXYYYYYZZ", "Can u provide top 5 points from this video?", "Hindi","text-bison@001",0.2,256,0.95,40],
        ["XXXXXYYYYYZZ-eU", "whats the video all about?", "Hindi","text-bison@001",0.2,256,0.95,40],
        ["XXXXXYYYYYZZ", "whats the game video all about?", "Hindi","text-bison@001",0.2,256,0.95,40],
        ["XXXXXYYYYYZZ", "whats the significance of this video?", "Hindi","text-bison@001",0.2,256,0.95,40],
        ["IIYLzb-XXXXXYYYYYZZ", "whats the significance of this video?", "Hindi","text-bison@001",0.2,256,0.95,40], 
        ["4-XXXXXYYYYYZZ", "Analyze the video's significance. List its key points, provide feedback, and check if google cloud case study from the the link https://cloud.google.com/blog/products/gcp/airports-of-thailand-and-evme-rely-on-cloud-to-improve-travel is similar to the video", "Hindi","text-bison@001",0.2,256,0.95,40], 
    ],
).launch()