all: USBMain.png USBMain.pdf USBSendShortToken.png USBSendShortToken.pdf

clean:
	rm *.png *.pdf

%.png:	%.dot
	dot -Tpng $^ -o $@ 

%.pdf:	%.dot
	dot -Tpdf $^ -o $@


