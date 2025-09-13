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
=== Website Blocker (Running as Administrator) === Commands: block <domain>    
- Block a website unblock <domain>
- Unblock a website list
- Show blocked domains exit
- Exit program
block example.com Successfully blocked: example.com list Blocked domains: example.com www.example.com unblock example.com Successfully unblocked: example.com exit
## Building

### Prerequisites

- Windows 10 or later
- Visual Studio 2022 (or compatible C++17 compiler)
- Administrator privileges to run

### Steps

1. Open the solution in Visual Studio.
2. Build the project (`Build > Build Solution`).
3. Run the executable.  
   > **Note:** The application will prompt for administrator rights if not already running as admin.

## How to Debug as Administrator

To debug the application with admin rights:
1. Close Visual Studio.
2. Right-click the Visual Studio shortcut and select **Run as administrator**.
3. Open the solution and start debugging as usual.

## How It Edits the Hosts File

- The program adds blocked domains between these markers in the hosts file:
Website Blocker - Blocked Sites
...
Website Blocker - End
- Each blocked domain is redirected to `127.0.0.1`.

## Disclaimer

- **Use at your own risk.** Modifying the hosts file can affect system networking.
- Always run this program as an administrator.
- The program is intended for educational and personal use.

## License

This project is licensed under the MIT License.

