FROM ros:melodic

RUN apt-get update && apt-get -y install python-bloom libi2c-dev && apt-get -y autoremove && apt-get -y clean && rm -rf /var/lib/apt/lists/*

COPY krex.sh /usr/local/bin/krex-run
VOLUME /krex
WORKDIR /krex
ENTRYPOINT ["/usr/local/bin/krex-run"]
