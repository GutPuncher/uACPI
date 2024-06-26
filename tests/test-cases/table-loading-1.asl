// Name: Load tables from opregions & fields
// Expect: str => Hello World 123

DefinitionBlock ("", "SSDT", 2, "uTEST", "TESTTABL", 0xF0F0F0F0)
{
    Method (MAIN, 0, Serialized)
    {
        Name (WRLD, "World")

        /*
         * Method (PRT0, 0, NotSerialized)
         * {
         *     Return(Concatenate("Hello ", \MAIN.WRLD))
         * }
         */
        External(\PRT0, MethodObj)
        Name (TAB0, Buffer {
            0x53,0x53,0x44,0x54,0x40,0x00,0x00,0x00,  /* 00000000    "SSDT@..." */
            0x02,0x86,0x75,0x54,0x45,0x53,0x54,0x00,  /* 00000008    "..uTEST." */
            0x54,0x45,0x53,0x54,0x54,0x41,0x42,0x4C,  /* 00000010    "TESTTAB0" */
            0xF0,0xF0,0xF0,0xF0,0x49,0x4E,0x54,0x4C,  /* 00000018    "....INTL" */
            0x31,0x03,0x22,0x20,0x14,0x1B,0x50,0x52,  /* 00000020    "1." ..PR" */
            0x54,0x30,0x00,0xA4,0x73,0x0D,0x48,0x65,  /* 00000028    "T0..s.He" */
            0x6C,0x6C,0x6F,0x20,0x00,0x5C,0x2E,0x4D,  /* 00000030    "llo .\.M" */
            0x41,0x49,0x4E,0x57,0x52,0x4C,0x44,0x00   /* 00000038    "AINWRLD." */
        })
        OperationRegion(TABR, SystemMemory, 0xDEADBEE0, SizeOf(TAB0))
        Field (TABR, WordAcc, NoLock, WriteAsOnes) {
            COPY, 512,
        }
        COPY = TAB0

        Load(TABR, Local0)
        If (!Local0) {
            Return ("Table 0 load failed")
        }

        /*
         * Name (O123, "123")
         *
         * Method (PRT1, 2, NotSerialized)
         * {
         *     Return(Concatenate(Concatenate(Arg0, " "), Arg1))
         * }
         */
        External(\PRT1, MethodObj)
        External(\O123, StrObj)
        Name (TAB1, Buffer {
            0x53,0x53,0x44,0x54,0x3F,0x00,0x00,0x00,  /* 00000000    "SSDT?..." */
            0x02,0x97,0x75,0x54,0x45,0x53,0x54,0x00,  /* 00000008    "..uTEST." */
            0x54,0x45,0x53,0x54,0x54,0x41,0x42,0x4C,  /* 00000010    "TESTTAB1" */
            0xF0,0xF0,0xF0,0xF0,0x49,0x4E,0x54,0x4C,  /* 00000018    "....INTL" */
            0x31,0x03,0x22,0x20,0x08,0x4F,0x31,0x32,  /* 00000020    "1." .O12" */
            0x33,0x0D,0x31,0x32,0x33,0x00,0x14,0x10,  /* 00000028    "3.123..." */
            0x50,0x52,0x54,0x31,0x02,0xA4,0x73,0x73,  /* 00000030    "PRT1..ss" */
            0x68,0x0D,0x20,0x00,0x00,0x69,0x00,0x00   /* 00000038    "h. ..i."  */
        })
        OperationRegion(TABX, SystemMemory, 0xCAFEBAB0, SizeOf(TAB1))
        Field (TABX, DWordAcc, NoLock, Preserve) {
            BLOB, 512,
        }

        // Copy the table into the operation region
        BLOB = TAB1

        Load(BLOB, Local1)
        If (!Local1) {
            Return ("Table 1 load failed")
        }

        Local0 = PRT0()
        Return (PRT1(Local0, O123))
    }

    Debug = MAIN()
}
