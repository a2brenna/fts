@0xea152d72be2589fb;

struct Manifest {

    manifestEntries @0 :List(Entry);

    struct Entry {
        key @0 :Text;
        dataRef @1 :Data;
        indexRef @2 :Data;
    }

    metadataCacheReflog @1 :Data;

}

struct MetadataCache {

    metadataReferences @0 :List(Entry);

    struct Entry {
        key @0 :Text;
        numElements @1 :UInt64;
        numBytes @2 :UInt64;
        lastTimestamp @3 :UInt64;
    }

}
