################################################################################
# Makefile for generating, playing, and viewing a tune
################################################################################

play: tune.mp3
	mpg123 --buffer 1024 tune.mp3 

view: tune.pdf
	gnome-open $^

clean:
	-rm tune.abc tune.ps tune.pdf tune.mid tune.wav tune.mp3 tune.tar

tune.tar: tune.abc tune.mp3 tune.pdf
	tar -cvf tune.tar tune.abc tune.mp3 tune.pdf

################################################################################
# Sounds
################################################################################

tune.mp3: tune.wav
	ffmpeg -y -i tune.wav tune.mp3

tune.wav: tune.mid
	timidity tune.mid -Ow

tune.mid: tune.abc
	abc2midi tune.abc
	-mv `ls -rS tune?.mid | tail -n 1` tmp.mid
	-ls tune?.mid | xargs rm
	mv tmp.mid tune.mid

################################################################################
# Notation
################################################################################

tune.pdf: tune.ps
	ps2pdf tune.ps tune.pdf

tune.ps: tune.abc
	abcm2ps tune.abc -O tune.ps

################################################################################
# Generation
################################################################################

tune.abc:
	./tradgen

.PHONY: play view clean
