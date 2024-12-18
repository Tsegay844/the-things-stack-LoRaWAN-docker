FROM python:3.9-slim

ENV PYTHONUNBUFFERED 1

WORKDIR /app

COPY ["requirements.txt", "."]

RUN pip install --no-cache-dir -r requirements.txt
#copy the whole WORKDIR 
COPY . .

CMD ["python", "data_retrive/main.py"]

