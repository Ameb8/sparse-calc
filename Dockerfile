
FROM alpine:latest

RUN apk add --no-cache \
    gcc \
    make \
    musl-dev \
    sqlite \
    sqlite-dev \
    bash

WORKDIR /workspace

CMD [ "bash" ]
