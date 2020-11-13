FROM alpine:3.12

# Install Base and utilities
RUN apk add --no-cache curl
RUN echo 'alias ll='"'"'ls $LS_OPTIONS -al'"'"'' >> ~/.ashrc
ENV ENV='.ashrc ash'

# Install arduino-cli
RUN curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh

# Create Config File
RUN arduino-cli config init
COPY arduino-cli.yaml /root/.arduino15

# Update local cache of available platforms and libraries 
RUN arduino-cli core update-index

# Install board
RUN arduino-cli core install esp8266:esp8266

WORKDIR /app

#Copy over project files
COPY main/test.ino ./


ENTRYPOINT [ "/bin/sh" ]