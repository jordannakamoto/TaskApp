import openai

def gpt4_prompt(prompt, api_key):
    openai.api_key = api_key
    response = openai.Completion.create(
        model="text-davinci-004",  # Replace with the GPT-4 model when it's available
        prompt=prompt,
        max_tokens=150
    )
    return response.choices[0].text.strip()

def whisper_transcription(audio_file_path, api_key):
    openai.api_key = api_key
    response = openai.Audio.transcribe(
        model="whisper-v1",  # Replace with the specific Whisper model you want to use
        audio=open(audio_file_path, "rb"),
        language='en'  # Replace with the desired language if not English
    )
    return response.data['text']

# Example usage
api_key = "your-api-key"  # Replace with your actual API key
print(gpt4_prompt("Translate the following English text to French: 'Hello, world!'", api_key))
print(whisper_transcription("path/to/audio/file.mp3", api_key))
