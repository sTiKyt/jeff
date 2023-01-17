#!/usr/bin/env python

import locale, os, openai, credentials, uuid

lang, _ = locale.getdefaultlocale()
KEEP_QNA = 9

if lang == "ru_RU":
  os.environ["http_proxy"] = credentials.HTTP_PROXY
  os.environ["https_proxy"] = credentials.HTTPS_PROXY
openai.api_key = credentials.API_KEY

ai_sequence = "\nAI: "
human_sequence = "\nHuman: "

prompt = "The following is a conversation with an AI assistant. The assistant is helpful, creative, clever, and very friendly.\n\nHuman: Hello, who are you?\nAI: I am Jeff created by Mark CDA and Victor Shamshin. I use an AI model created by OpenAI. How can I help you today?"

def generate_response(prompt):
  """"""
  try:
    completions = openai.Completion.create(
      model="text-davinci-003",
      prompt=prompt,
      temperature=0.9,
      max_tokens=1024,
      top_p=1,
      frequency_penalty=0,
      presence_penalty=0.6,
      stop=["Human:", "AI:"]
    )
    message = completions.choices[0].text
    return message
  except openai.error.RateLimitError:
    return "*[ChatGPT]* Server is overloaded. Try again later." if lang != 'ru_RU' else "[ChatGPT]* Сервер перегружен. Попробуйте повторить попытку позже."
  except openai.error.ServiceUnavailableError:
    return "*[ChatGPT]* Server is unaccessible. Try again using proxy." if lang != 'ru_RU' else "[ChatGPT]* Сервер недоступен. Попробуйте использовать прокси."
  except openai.error.APIConnectionError:
    return "*[ChatGPT]* Unable to connect to API. Try to change proxy." if lang != 'ru_RU' else "[ChatGPT]* Не удалось подключиться к API. Попробуйте сменить прокси."


def history_reducer(history):
  """"""
  if len(history) > KEEP_QNA >= 3:
    del history[2]
    del history[1]
  return history


def make_prompt(history):
  return ''.join(history) + ai_sequence


def main():
  """"""
  from jeff_api import client, server
  
  srv = server.Server(None, 23174)
  cli = client.Client('localhost', 8005)
  
  history = [prompt]
  
  print('ChatGPT activated.' if lang != 'ru_RU' else 'ChatGPT активирован.')
  while True:
    data = srv.listen()
    if len(data) == 0:
      continue
    if data['author'] == 1: # Jeff
      continue
    if len(data['content']) < 10:
      continue
    msg_id = str(uuid.uuid4())
    cli.send_status(msg_id, '*[ChatGPT]* Waiting...' if lang != 'ru_RU' else '*[ChatGPT]* Ожидание...')
    history.append(human_sequence + data['content'])
    response = generate_response(make_prompt(history)).strip()
    if len(response) == 0:
      cli.send_status(msg_id, '')
    history.append(response)
    cli.send_status(msg_id, response.replace(ai_sequence, ''))
    history = history_reducer(history)


if __name__ == '__main__':
  try:
    main()
  except KeyboardInterrupt:
    print('\nChatGPT disabled.' if lang != 'ru_RU' else '\nChatGPT отключён.')