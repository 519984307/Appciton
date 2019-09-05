
# auto run target after build
test {
QMAKE_POST_LINK=xvfb-run ./$$TARGET
}
