/*
 * Copyright (C) 2025 Alexander Parvanov and Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <ctype.h>

#define PORT 30303
#define BUFSIZE 1024
#define MAX_CMD_LEN 256

const char *ac[] = {
	"brave_focus",
	"brave_playpause",
	"brave_netflix_next",
	"volume_up",
	"volume_down"
};

const int c = sizeof(ac) / sizeof(ac[0]);

int a(const char *cmd) {
	for (int i = 0; i < c; i++) {
		if (strncmp(cmd, ac[i], strlen(ac[i])) == 0 && strlen(cmd) == strlen(ac[i])) {
			return 1;
		}
	}
	return 0;
}

void url_decode(char *dst, const char *src) {
	char a, b;
	while (*src) {
		if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {
			if (a >= 'a') a -= 'a' - 'A';
			if (a >= 'A') a -= ('A' - 10);
			else a -= '0';
			if (b >= 'a') b -= 'a' - 'A';
			if (b >= 'A') b -= ('A' - 10);
			else b -= '0';
			*dst++ = 16 * a + b;
			src += 3;
		} else if (*src == '+') {
			*dst++ = ' ';
			src++;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';
}

const char *get_apple_script_command(const char *cmd) {
	if (strcmp(cmd, "brave_focus") == 0) {
		return "osascript -e 'tell application \"System Events\" to if (name of processes) contains \"Brave Browser\" then tell application \"Brave Browser\" to activate else tell application \"Brave Browser\" to launch delay 1 tell application \"Brave Browser\" to activate end if end tell'";
	} else if (strcmp(cmd, "brave_playpause") == 0) {
		return "osascript -e 'tell application \"System Events\" to keystroke space' -e 'tell application \"Brave Browser\" to activate'";
	} else if (strcmp(cmd, "brave_netflix_next") == 0) {
		return "osascript -e 'tell application \"Brave Browser\" to activate' -e 'tell application \"Brave Browser\" to tell front window to tell active tab to do JavaScript \"var nextBtn = document.querySelector(\\\"button[data-uia=\\\\\\\"control-next\\\\\\\"]\\\"); if(nextBtn) { nextBtn.click(); }\"'";
	} else if (strcmp(cmd, "volume_up") == 0) {
		return "osascript -e 'set currentVolume to output volume of (get volume settings)' -e 'if currentVolume < 100 then set volume output volume (currentVolume + 5) end if'";
	} else if (strcmp(cmd, "volume_down") == 0) {
		return "osascript -e 'set currentVolume to output volume of (get volume settings)' -e 'if currentVolume > 0 then set volume output volume (currentVolume - 5) end if'";
	}
	return NULL;
}

const char *lan_ip_address(void) {
	static char ip[INET_ADDRSTRLEN];
	struct ifaddrs *ifaddr = NULL;
	if (getifaddrs(&ifaddr) == -1) {
		return NULL;
	}
	for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (!ifa->ifa_addr || !(ifa->ifa_flags & IFF_UP)) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET && !(ifa->ifa_flags & IFF_LOOPBACK)) {
			struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
			if (!inet_ntop(AF_INET, &sa->sin_addr, ip, sizeof(ip))) {
				ip[0] = '\0';
				break;
			}
			break;
		}
	}
	freeifaddrs(ifaddr);
	return ip[0] ? ip : NULL;
}

int main() {
	struct sockaddr_in server_addr;
	int server_sock, client_sock;
	char buf[BUFSIZE + 1];
	char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0) {
		perror("socket");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		exit(1);
	}
	listen(server_sock, 1);
	const char *ip = lan_ip_address();
	if (ip) {
		printf("go to: %s:%d\n", ip, PORT);
	} else {
		printf("go to: <local-network-ip>:%d\n", PORT);
	}
	while (1) {
		client_sock = accept(server_sock, NULL, NULL);
		if (client_sock < 0) {
			perror("accept");
			continue;
		}
		int received = read(client_sock, buf, BUFSIZE);
		if (received <= 0) {
			close(client_sock);
			continue;
		}
		buf[received] = 0;
		if (strncmp(buf, "GET ", 4) == 0) {
			char *cmd_pos = strstr(buf, "GET /cmd?cmd=");
			if (cmd_pos) {
				char *cmd_start = cmd_pos + strlen("GET /cmd?cmd=");
				char *cmd_end = strchr(cmd_start, ' ');
				if (!cmd_end) cmd_end = cmd_start + strlen(cmd_start);
				char raw_cmd[MAX_CMD_LEN];
				int len = cmd_end - cmd_start;
				if (len > MAX_CMD_LEN - 1) len = MAX_CMD_LEN - 1;
				strncpy(raw_cmd, cmd_start, len);
				raw_cmd[len] = '\0';
				char cmd_decoded[MAX_CMD_LEN];
				url_decode(cmd_decoded, raw_cmd);
				if (a(cmd_decoded)) {
					const char *os_cmd = get_apple_script_command(cmd_decoded);
					if (os_cmd != NULL) {
						FILE *fp = popen(os_cmd, "r");
						if (fp) {
							write(client_sock, header, sizeof(header) - 1);
							char outbuf[BUFSIZE];
							while (fgets(outbuf, sizeof(outbuf), fp)) {
								write(client_sock, outbuf, strlen(outbuf));
							}
							pclose(fp);
						} else {
							char *err = "<h1>Failed to execute command</h1>";
							write(client_sock, header, sizeof(header) - 1);
							write(client_sock, err, strlen(err));
						}
					} else {
						char *err = "<h1>Command not allowed</h1>";
						write(client_sock, header, sizeof(header) - 1);
						write(client_sock, err, strlen(err));
					}
				} else {
					char *err = "<h1>Command not allowed</h1>";
					write(client_sock, header, sizeof(header) - 1);
					write(client_sock, err, strlen(err));
				}
			} else if (strstr(buf, "GET / ") || strstr(buf, "GET /index.html ")) {
				write(client_sock, header, sizeof(header) - 1);
				FILE *fp = fopen("index.html", "r");
				if (fp) {
					int r;
					while ((r = fread(buf, 1, BUFSIZE, fp)) > 0)
						write(client_sock, buf, r);
					fclose(fp);
				} else {
					char *notfound = "<h1>404 Not Found</h1>";
					write(client_sock, notfound, strlen(notfound));
				}
			} else {
				char *notfound = "<h1>404 Not Found</h1>";
				write(client_sock, header, sizeof(header) - 1);
				write(client_sock, notfound, strlen(notfound));
			}
		} else {
			char *notfound = "<h1>Only GET supported</h1>";
			write(client_sock, header, sizeof(header) - 1);
			write(client_sock, notfound, strlen(notfound));
		}
		close(client_sock);
	}
	close(server_sock);
	return 0;
}
