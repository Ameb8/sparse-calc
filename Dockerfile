
FROM debian:bookworm-slim

RUN apt update && apt install -y \
    gcc \
    make \
    sqlite3 \
    libsqlite3-dev \
    bash \
    gdb \
    valgrind \
    strace \
    && apt clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD [ "bash" ]
