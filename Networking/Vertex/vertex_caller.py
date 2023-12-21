import vertexai
from vertexai.language_models import TextGenerationModel
from youtube_transcript_api import YouTubeTranscriptApi

vertexai.init(project="your_project_id", location="us-central1")
api_key = XXXXXXX

def get_transcript(video_id):
    try:
        transcript = YouTubeTranscriptApi.get_transcript(video_id)
        return " ".join([t['text'] for t in transcript]).replace("\n", " ")
    except Exception as e:
        return str(e)

def generate_tasks(video_link):
    # Extract video ID from link (assuming YouTube video links)
    video_id = video_link.split("watch?v=")[-1]
    
    # Retrieve transcript
    transcript = get_transcript(video_id)
    if not transcript:
        return "Transcript not available for this video."

    # Formulate prompt
    prompt = "Can you take this video transcript and break it into 8 tasks with 3-5 subtasks each. The format should be '1. Task 1' for each task title. Don't include timestamps or any other information. Here is the transcript: " + transcript

    # Call the language model
    model = TextGenerationModel.from_pretrained("model_name_here")
    parameters = {
        "temperature": 0.2,
        "max_output_tokens": 256,
        "top_p": 0.95,
        "top_k": 40
    }
    response = model.predict(prompt, **parameters)
    return response.text

# Example usage
youtube_link = "https://www.youtube.com/watch?v=example_video_id"
tasks_and_subtasks = generate_tasks(youtube_link)
print(tasks_and_subtasks)
