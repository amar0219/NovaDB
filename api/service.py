import json
import subprocess
from pathlib import Path


ROOT_DIR = Path(__file__).resolve().parent.parent
CLI = ROOT_DIR / "novadb_cli.exe"


class NovaDBService:

    @staticmethod
    def run_command(args):

        command = [str(CLI)] + args

        result = subprocess.run(
            command,
            capture_output=True,
            text=True
        )

        print("COMMAND:", command)
        print("RETURN CODE:", result.returncode)
        print("STDOUT:")
        print(repr(result.stdout))
        print("STDERR:")
        print(repr(result.stderr))

        return json.loads(result.stdout)

    @staticmethod
    def insert(embedding):

        args = ["insert"]

        args.extend(str(x) for x in embedding)

        return NovaDBService.run_command(args)

    @staticmethod
    def search(query, k):

        args = ["search", str(k)]

        args.extend(str(x) for x in query)

        return NovaDBService.run_command(args)

    @staticmethod
    def get(vector_id):

        return NovaDBService.run_command(
            ["get", str(vector_id)]
        )

    @staticmethod
    def delete(vector_id):

        return NovaDBService.run_command(
            ["delete", str(vector_id)]
        )