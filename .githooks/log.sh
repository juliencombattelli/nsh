black="\e[0;30m"
red="\e[0;31m"
green="\e[0;32m"
yellow="\e[0;33m"
blue="\e[0;34m"
purple="\e[0;35m"
cyan="\e[0;36m"
white="\e[0;37m"

reset="\e[0m"
bolc="\e[1m"
dim="\e[2m"
italic="\e[3m"
underline="\e[4m"
blink="\e[5m"
inverse="\e[7m"
hidden="\e[8m"
strikethrough="\e[9m"

log() {
    echo "$@${reset}"
}
