FROM python:3.13-slim

# Install global dependencies
RUN apt-get update && apt-get install -y curl gnupg supervisor \
&& curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
&& apt-get install -y nodejs \
&& apt-get clean && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

EXPOSE 3000 5000 8888

# Copy requirements
COPY PoopBiter/requirements.txt ./PoopBiter/
COPY Cherry/requirements.txt ./Cherry/
COPY CornCake/requirements.txt ./CornCake/
COPY PassionFruit/requirements.txt ./PassionFruit/
COPY PassionFruit/frontend/package.json ./PassionFruit/frontend/

# Install dependencies
RUN pip install -r PoopBiter/requirements.txt \
                -r Cherry/requirements.txt \
                -r CornCake/requirements.txt \
                -r PassionFruit/requirements.txt

RUN cd PassionFruit/frontend && npm install

# Copy runnable sources
COPY PoopBiter ./PoopBiter/
COPY Cherry ./Cherry
COPY CornCake ./CornCake
COPY PassionFruit ./PassionFruit
COPY ./supervisord.conf ./

# Run the apps in parallel
CMD ["supervisord", "-c", "/app/supervisord.conf"]
