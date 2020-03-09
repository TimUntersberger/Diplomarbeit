FROM node:alpine3.10
RUN npm i -g serve
WORKDIR /app
COPY dist/ .
CMD serve -s
