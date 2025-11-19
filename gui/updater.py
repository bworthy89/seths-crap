"""
Auto-updater module for Arduino Input Configurator

Checks GitHub for new releases and downloads/installs updates automatically.
"""

import os
import sys
import json
import urllib.request
import urllib.error
import zipfile
import shutil
import tempfile
import subprocess
from pathlib import Path


class Updater:
    def __init__(self, repo_owner="bworthy89", repo_name="seths-crap", current_version=None):
        """
        Initialize the updater.

        Args:
            repo_owner (str): GitHub repository owner
            repo_name (str): GitHub repository name
            current_version (str): Current version (e.g., "1.0.0")
        """
        self.repo_owner = repo_owner
        self.repo_name = repo_name
        self.project_root = Path(__file__).parent.parent.absolute()
        self.current_version = current_version or self._read_version_file()
        self.github_api_url = f"https://api.github.com/repos/{repo_owner}/{repo_name}/releases/latest"

    def _read_version_file(self):
        """Read current version from VERSION file."""
        version_file = self.project_root / "VERSION"
        try:
            if version_file.exists():
                return version_file.read_text().strip()
        except Exception:
            pass
        return "0.0.0"

    def check_for_updates(self, timeout=10):
        """
        Check if a new version is available on GitHub.

        Args:
            timeout (int): Request timeout in seconds

        Returns:
            dict: Update info with keys:
                - available (bool): Whether update is available
                - latest_version (str): Latest version on GitHub
                - current_version (str): Current installed version
                - download_url (str): Download URL for new version
                - release_notes (str): Release notes
            None if check failed
        """
        try:
            # Query GitHub API for latest release
            req = urllib.request.Request(self.github_api_url)
            req.add_header('User-Agent', 'Arduino-Input-Configurator')

            with urllib.request.urlopen(req, timeout=timeout) as response:
                data = json.loads(response.read().decode())

            latest_version = data['tag_name'].lstrip('v')  # Remove 'v' prefix if present
            download_url = None

            # Find the source code zip URL
            for asset in data.get('assets', []):
                if asset['name'].endswith('.zip'):
                    download_url = asset['browser_download_url']
                    break

            # If no asset, use the source code archive
            if not download_url:
                download_url = data.get('zipball_url')

            # Compare versions
            update_available = self._compare_versions(latest_version, self.current_version) > 0

            return {
                'available': update_available,
                'latest_version': latest_version,
                'current_version': self.current_version,
                'download_url': download_url,
                'release_notes': data.get('body', 'No release notes available.'),
                'html_url': data.get('html_url', '')
            }

        except urllib.error.HTTPError as e:
            if e.code == 404:
                # No releases yet
                return {
                    'available': False,
                    'latest_version': self.current_version,
                    'current_version': self.current_version,
                    'download_url': None,
                    'release_notes': 'No releases available on GitHub yet.',
                    'html_url': f'https://github.com/{self.repo_owner}/{self.repo_name}/releases'
                }
            return None
        except Exception as e:
            print(f"Update check failed: {e}")
            return None

    def _compare_versions(self, version1, version2):
        """
        Compare two semantic versions.

        Args:
            version1 (str): First version (e.g., "1.2.3")
            version2 (str): Second version (e.g., "1.2.0")

        Returns:
            int: 1 if version1 > version2, -1 if version1 < version2, 0 if equal
        """
        def parse_version(v):
            parts = v.split('.')
            return tuple(int(p) for p in parts[:3])  # Major.Minor.Patch

        try:
            v1 = parse_version(version1)
            v2 = parse_version(version2)

            if v1 > v2:
                return 1
            elif v1 < v2:
                return -1
            else:
                return 0
        except Exception:
            return 0

    def download_and_install_update(self, download_url, callback=None):
        """
        Download and install an update from GitHub.

        Args:
            download_url (str): URL to download the update from
            callback (callable): Optional callback function(progress, status)
                                progress: 0-100 percent
                                status: status message string

        Returns:
            bool: True if update successful, False otherwise
        """
        try:
            if callback:
                callback(0, "Downloading update...")

            # Download to temporary directory
            temp_dir = tempfile.mkdtemp(prefix='arduino_update_')
            zip_path = os.path.join(temp_dir, 'update.zip')

            # Download with progress
            def download_progress(block_num, block_size, total_size):
                if callback and total_size > 0:
                    percent = min(100, int((block_num * block_size / total_size) * 50))
                    callback(percent, f"Downloading... {percent}%")

            urllib.request.urlretrieve(download_url, zip_path, download_progress)

            if callback:
                callback(50, "Extracting files...")

            # Extract zip
            extract_dir = os.path.join(temp_dir, 'extracted')
            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                zip_ref.extractall(extract_dir)

            # Find the root directory in the extracted files
            extracted_items = os.listdir(extract_dir)
            if len(extracted_items) == 1 and os.path.isdir(os.path.join(extract_dir, extracted_items[0])):
                source_dir = os.path.join(extract_dir, extracted_items[0])
            else:
                source_dir = extract_dir

            if callback:
                callback(60, "Installing update...")

            # Copy files to project directory (excluding user data)
            self._copy_update_files(source_dir, self.project_root, callback)

            if callback:
                callback(90, "Cleaning up...")

            # Clean up temp directory
            shutil.rmtree(temp_dir, ignore_errors=True)

            if callback:
                callback(100, "Update complete!")

            return True

        except Exception as e:
            print(f"Update installation failed: {e}")
            if callback:
                callback(0, f"Update failed: {str(e)}")
            return False

    def _copy_update_files(self, source_dir, dest_dir, callback=None):
        """
        Copy update files, excluding user data and system files.

        Args:
            source_dir (str): Source directory with new files
            dest_dir (Path): Destination directory (project root)
            callback (callable): Progress callback
        """
        # Files/directories to skip (user data, system files)
        skip_items = {
            '.git', '.github', '.gitignore', '.vs', '.vscode', '.pio',
            '__pycache__', '*.pyc', '*.pyo', 'venv', 'env',
            '.claude'  # Don't overwrite Claude workspace
        }

        # Copy files recursively
        total_files = sum(1 for _ in Path(source_dir).rglob('*') if _.is_file())
        copied_files = 0

        for source_path in Path(source_dir).rglob('*'):
            if source_path.is_file():
                # Get relative path
                rel_path = source_path.relative_to(source_dir)

                # Skip if in skip list
                if any(skip in rel_path.parts for skip in skip_items):
                    continue

                # Destination path
                dest_path = dest_dir / rel_path

                # Create parent directories
                dest_path.parent.mkdir(parents=True, exist_ok=True)

                # Copy file
                shutil.copy2(source_path, dest_path)

                copied_files += 1
                if callback and total_files > 0:
                    progress = 60 + int((copied_files / total_files) * 30)
                    callback(progress, f"Installing files... ({copied_files}/{total_files})")

    def restart_application(self):
        """
        Restart the application after update.

        Returns:
            bool: True if restart initiated
        """
        try:
            # Get the path to the launcher script
            launcher = self.project_root / "launch-gui.bat"

            if launcher.exists():
                # Launch the application
                subprocess.Popen([str(launcher)], shell=True)
                return True
            else:
                # Fallback: restart Python script
                python = sys.executable
                script = Path(__file__).parent / "main.py"
                subprocess.Popen([python, str(script)])
                return True

        except Exception as e:
            print(f"Restart failed: {e}")
            return False


def check_for_updates_cli():
    """Command-line interface for checking updates."""
    updater = Updater()

    print("Checking for updates...")
    update_info = updater.check_for_updates()

    if update_info is None:
        print("Failed to check for updates. Please check your internet connection.")
        return

    if not update_info['available']:
        print(f"You have the latest version ({update_info['current_version']}).")
        return

    print(f"\nUpdate available!")
    print(f"Current version: {update_info['current_version']}")
    print(f"Latest version:  {update_info['latest_version']}")
    print(f"\nRelease notes:")
    print(update_info['release_notes'])

    # Ask user if they want to update
    response = input("\nDo you want to install this update? (yes/no): ")
    if response.lower() not in ['yes', 'y']:
        print("Update cancelled.")
        return

    # Download and install
    def progress_callback(percent, status):
        print(f"\r{status} [{percent}%]", end='', flush=True)

    print("\nInstalling update...")
    success = updater.download_and_install_update(
        update_info['download_url'],
        callback=progress_callback
    )

    if success:
        print("\n\nUpdate installed successfully!")
        print("Please restart the application.")
    else:
        print("\n\nUpdate failed. Please try again or download manually.")


if __name__ == '__main__':
    check_for_updates_cli()
