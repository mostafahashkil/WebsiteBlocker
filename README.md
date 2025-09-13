# WebsiteBlocker

A simple C++17 console application for Windows that allows you to block and unblock websites by editing the system `hosts` file. The application requires administrator privileges to function, as it modifies system files and flushes the DNS cache.

## Features

- **Block websites** by domain name (including `www.` and non-`www.` variants)
- **Unblock websites** easily
- **List all currently blocked domains**
- **Automatic DNS cache flush** after changes
- **Administrator privilege check** and auto-restart with UAC prompt if needed

## How It Works

The application modifies the Windows `hosts` file to redirect specified domains to `127.0.0.1`, effectively blocking them. It maintains a clearly marked section in the `hosts` file for easy management.

## Usage

### Commands

- `block <domain>` &mdash; Block a website (e.g., `block example.com`)
- `unblock <domain>` &mdash; Unblock a website (e.g., `unblock example.com`)
- `list` &mdash; Show all currently blocked domains
- `exit` &mdash; Exit the program

### Example Session
# WebsiteBlocker
