BEGIN {print "/**"; print " * @mainpage"}
{print " * "  $0}
END { print " */" }


