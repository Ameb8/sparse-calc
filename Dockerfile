
FROM debian:bookworm-slim

RUN apt update && apt install -y \
    gcc \
    make \
    sqlite3 \
    libsqlite3-dev \
    bash

WORKDIR /workspace

CMD [ "bash" ]
