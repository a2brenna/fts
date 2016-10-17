@0xf32d8819b6589d3e;

struct Request {

    key @0 :Text;

    union {

        append :group {
            data @1 :Text;
            timeSinceEpoch @2 :UInt64;
        }

        query :group{
            lastn @3 :UInt64;
            startt @4 :UInt64;
            endt @5 :UInt64;
            starti @6 :UInt64;
            endi @7 :UInt64;
        }

    }
}

struct Response {

    return @0 :ReturnCode;
    bytesToFollow @1 :UInt64;

    #This will get more complicated as we propogate exceptions from the
    #Server to the Client
    enum ReturnCode {
        success @0;
        fail @1;
    }

}
