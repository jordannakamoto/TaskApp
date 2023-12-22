import sys
import requests
from bardapi import Bard

import requests
from bardapi.constants import SESSION_HEADERS
from bardapi import Bard

token = "notworking"

session = requests.Session()
session.headers = SESSION_HEADERS
session.cookies.set("__Secure-1PSID", token)
session.cookies.set("__Secure-1PSIDTS", "notworking")
session.cookies.set("__Secure-1PSIDCC", "notworking")

def call_bard(youtubelink):
   query = "can you take this video: " + youtubelink + "and break it into 8 tasks with 3-5 subtasks each. The format should be '1. Task 1' for each task title don't include timestamps in your response, or any other information or replies to me besides the tasks and subtasks"
   bard = Bard(token=token, session=session)
   answer = bard.get_answer(query)
   return (answer['content'])

# def send_to_bard(input_string, prompt):
#     # Combine the input string with the prompt
#     full_prompt = f"{prompt} {input_string}"

#     # Call the Bard API
#     response = requests.post('https://bard.api.url', json={'prompt': full_prompt})

#     # Return the API response
#     return response.text  # or response.json(), as appropriate

if __name__ == "__main__":
   response = call_bard("https://www.youtube.com/watch?v=NlXfg5Pxxh8&t")
   print(response)
    # input_string = sys.argv[1]
    # prompt = "Your preconfigured prompt"
    # result = send_to_bard(input_string, prompt)
    # print(result)
