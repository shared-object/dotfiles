import subprocess
import shlex

def execute(command: str) -> str:
    splitted_command = shlex.split(command)

    result = subprocess.run(splitted_command, capture_output=True, text=True)

    return result.stdout

command = "echo \"$HOME/books/$(ls ~/books/ | fzf)\""

print(execute(command))
