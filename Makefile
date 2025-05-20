IMAGE_NAME = c-gcc-sqlite-alpine

build:
	docker build -t $(IMAGE_NAME) .

run:
	mkdir -p data
	docker run --rm -it -v $(PWD):/app -v $(PWD)/data:/app/data $(IMAGE_NAME)

clean:
	docker rmi $(IMAGE_NAME) || true
