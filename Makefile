.PHONY: encrypt decrypt default

default: encrypt

encrypt: exam.md.gpg

exam.md.gpg: exam.md
	cat exam.key | gpg --batch --yes --passphrase-fd 0 --output $@ --symmetric $^

decrypt: exam.md

exam.md: exam.md.gpg
	cat exam.key | gpg --batch --yes --passphrase-fd 0 --output $@ --decrypt $^ 

