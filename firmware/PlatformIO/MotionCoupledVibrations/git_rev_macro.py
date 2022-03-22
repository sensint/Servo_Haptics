import subprocess

revision = (
    subprocess.check_output(["git", "rev-parse", "HEAD"])
    .strip()
    .decode("utf-8")
)

latest_tag = (
    subprocess.check_output("git describe --tags $(git rev-list --tags --max-count=1)", shell=True)
    .strip()
    .decode("utf-8")
)

print("-DGIT_REV='\"%s\"' -DGIT_TAG='\"%s\"'" % (revision[0:7], latest_tag))
