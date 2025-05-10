$destination = "localhost:8888"
$url = "http://$destination/install"

# Fetch and invoke full installation script
Invoke-Expression (Invoke-WebRequest -Uri $url -UseBasicParsing).Content
