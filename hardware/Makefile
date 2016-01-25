#
# Makefile -- package the gerber files for manufacturing
#
# (c) 2016 Prof Dr Andreas Mueller, Hochschule Rapperswil
#
GERBERS = focuser-B_Cu.gbl focuser-B_Mask.gbs focuser-Edge_Cuts.gbr \
	focuser-F_Cu.gtl focuser-F_Mask.gts
DRILL = focuser.drl

focuser.zip:	$(GERBERS) $(DRILL)
	zip focuser.zip $(GERBERS) $(DRILL)


