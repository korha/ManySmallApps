Opens listening ports and redirects incoming traffic to a specific host

Rules must be stored in ini-file in format:
IPLocal:PortLocal|IPRemote:PortRemote.
E.g.: 0.0.0.0:1234|192.168.1.5:80 - listen 1234 port to all network interfaces and redirects to 192.168.1.5:80

Empty lines and lines starting on ";" ignored.
