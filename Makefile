.PHONY: encrypt decrypt default

default: encrypt

encrypt: container.tar.gz.gpg

container.tar.gz.gpg: container.hidden
	rm -f $@
	tar -cz $^ | gpg --batch --yes --passphrase-file container.key --output $@ --symmetric --

decrypt: container.hidden

container.hidden: container.tar.gz.gpg
	rm -rf $@
	gpg --batch --yes --passphrase-file container.key --decrypt $^ | tar -xz
