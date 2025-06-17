# XpMono
Lets gooooooo you scrolled down!

Means you're interested... 😉
This is about to be even bigger so give a star to not forget 😁


# Dependencies (Backend)
## Global Dependencies:
- nodejs
- python 3.13


## Python Dependencies:
```
pip install -r PoopBiter/requirements.txt -r Cherry/requirements.txt -r CornCake/requirements.txt -r PassionFruit/requirements.txt
```

## NodeJS Dependencies:
```
cd PassionFruit/frontend && npm install
```

# Run
## Local
Run the backend locally using the joint script:

(Windows)
`start_backend.bat`

(Linux)
`./start_backend.sh`

## Deploy
Deploy the entier backend in a single docker conatiner

1. Build docker container: `docker build -t xpmono:latest .`
2. Run docker container: `docker run -it -p 3000:3000 -p 5000:5000 -p 8888:8888 --rm xpmono`
3. Launch the Web interface using your favourite browser on http://localhost:3000
