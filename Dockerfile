FROM codenvy/cpp

MAINTAINER Vitor Enes <vitorenesduarte@gmail.com>

ENV NAME=exactly-once

RUN sudo apt-get update && sudo apt-get install -y git
RUN git clone https://github.com/vitorenesduarte/$NAME

WORKDIR $NAME

RUN make

CMD ["bin/main"]
