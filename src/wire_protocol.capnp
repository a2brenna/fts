@0xf32d8819b6589d3e;

struct Request {

    key @0 :Text;

    union {

        append :group {
            data @1 :Text;
            timeSinceEpoch @2 :UInt64;
        }

        query :group{
            youngest @3 :UInt64;
            oldest @4 :UInt64;
            minIndex @5 :UInt64;
            maxIndex @6 :UInt64;
            tailSize @7 :UInt64;
            tailAge @8 :UInt64;
        }

    }
}

struct Response {

    return @0 :ReturnCode;

    #This will get more complicated as we propogate exceptions from the
    #Server to the Client
    enum ReturnCode {
        success @0;
        bytesToFollow @1;
        fail @2;
    }

}
