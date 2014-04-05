public class dhry {

    /* DhrystoneConstants.java stuff */
    public static final int Ident_1     = 0;
    public static final int Ident_2     = 1;
    public static final int Ident_3     = 2;
    public static final int Ident_4     = 3;
    public static final int Ident_5     = 4;

    /* GlobalVariables.java stuff */
    static dhry               Record_Glob,
                                Next_Record_Glob;
    static int                  Int_Glob;
    static boolean              Bool_Glob;
    static char                 Char_Glob_1,
                                Char_Glob_2;
    static int[]                Array_Glob_1    = new int[128];
    static int[][]              Array_Glob_2    = new int[128][128];
    static dhry               First_Record    = new dhry(),
                                Second_Record   = new dhry();
    
    /* Record_Type.java stuff */
    dhry Record_Comp;
    int Discr;
    int Enum_Comp;
    int Int_Comp;
    String String_Comp;
    int Enum_Comp_2;
    String String_Comp_2;
    char Char_Comp_1;
    char Char_Comp_2;

    /* dhry.java private fields */
    long Number_Of_Runs = 10000000;
    
    /* dhry.java method */
    public void execute() {

        int         Int_Loc_1,
                    Int_Loc_2,
                    Int_Loc_3;
        int[]       Int_Loc_3_Ref = new int[1];
        int[]       Int_Loc_1_Ref = new int[1];
        char        Char_Index;
        int[]       Enum_Loc = new int[1];
        String      String_Loc_1,
                    String_Loc_2;

        long        begin_time,
                    end_time,
                    total_time;

        int         Run_Index,
                    Meas_Index;

        Next_Record_Glob            = Second_Record;
        Record_Glob                 = First_Record;

        Record_Glob.Record_Comp     = Next_Record_Glob;
        Record_Glob.Discr           = Ident_1;
        Record_Glob.Enum_Comp       = Ident_3;
        Record_Glob.Int_Comp        = 40;
        Record_Glob.String_Comp     = "DHRYSTONE PROGRAM, SOME STRING";

        String_Loc_1                = "DHRYSTONE PROGRAM, 1'ST STRING";

        System.err.println("Execution starts, " + Number_Of_Runs
                        + " runs through Dhrystone");

        begin_time = System.currentTimeMillis();

        for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index) {

            Proc_5();
            Proc_4();

            Int_Loc_1 = 2;
            Int_Loc_2 = 3;

            String_Loc_2 = "DHRYSTONE PROGRAM, 2'ND STRING";

            Enum_Loc[0] = Ident_2;
            Bool_Glob = !Func_2(String_Loc_1, String_Loc_2);

            while (Int_Loc_1 < Int_Loc_2) {
                Int_Loc_3_Ref[0] = 5 * Int_Loc_1 - Int_Loc_2;
                Proc_7(Int_Loc_1, Int_Loc_2, Int_Loc_3_Ref);
                Int_Loc_1 += 1;
            }

            Int_Loc_3 = Int_Loc_3_Ref[0];
            Proc_8(Array_Glob_1, Array_Glob_2, Int_Loc_1, Int_Loc_3);
            Proc_1(Record_Glob);

            for (Char_Index = 'A'; Char_Index <= Char_Glob_2; ++Char_Index) {
                if (Enum_Loc[0] == Func_1(Char_Index, 'C'))
                    Proc_6(Ident_1, Enum_Loc);
            }
            
            Int_Loc_3 = Int_Loc_2 * Int_Loc_1;
            Int_Loc_2 = Int_Loc_3 / Int_Loc_1;
            Int_Loc_2 = 7 * (Int_Loc_3 - Int_Loc_2) - Int_Loc_1;

            Int_Loc_1_Ref[0] = Int_Loc_1;
            Proc_2(Int_Loc_1_Ref);
            Int_Loc_1 = Int_Loc_1_Ref[0];
            
        }

        end_time = System.currentTimeMillis();
        total_time = end_time - begin_time;

        System.err.println("total time: " + total_time + "ms");
        System.err.println("Result: " + Number_Of_Runs * 1000 / total_time
                        + " dhrystone/sec.");

    }

    /* dhry.java method */
    void Proc_1(dhry Pointer_Par_Val) {

        dhry Next_Record = Pointer_Par_Val.Record_Comp;

        Pointer_Par_Val.Record_Comp = Record_Glob;

        Pointer_Par_Val.Int_Comp = 5;

        Next_Record.Int_Comp = Pointer_Par_Val.Int_Comp;
        Next_Record.Record_Comp = Pointer_Par_Val.Record_Comp;
        Proc_3(Next_Record.Record_Comp);

        int[] Int_Ref = new int[1];

        if (Next_Record.Discr == Ident_1) {
            Next_Record.Int_Comp = 6;
            Int_Ref[0] = Next_Record.Enum_Comp;
            Proc_6(Pointer_Par_Val.Enum_Comp, Int_Ref);
            Next_Record.Enum_Comp = Int_Ref[0];
            Next_Record.Record_Comp = Record_Glob.Record_Comp;
            Int_Ref[0] = Next_Record.Int_Comp;
            Proc_7(Next_Record.Int_Comp, 10, Int_Ref);
            Next_Record.Int_Comp = Int_Ref[0];
        } else
            Pointer_Par_Val = Pointer_Par_Val.Record_Comp;

    }

    /* dhry.java method */
    void Proc_2(int Int_Par_Ref[]) {

        int Int_Loc;
        int Enum_Loc;

        Int_Loc = Int_Par_Ref[0] + 10;
        Enum_Loc = 0;

        do
            if (Char_Glob_1 == 'A') {
                Int_Loc -= 1;
                Int_Par_Ref[0] = Int_Loc - Int_Glob;
                Enum_Loc = Ident_1;
            }
        while (Enum_Loc != Ident_1);

    }

    /* dhry.java method */
    void Proc_3(dhry Pointer_Par_Ref) {

        if (Record_Glob != null)
            Pointer_Par_Ref = Record_Glob.Record_Comp;
        else
            Int_Glob = 100;

        int[] Int_Comp_Ref = new int[1];
        Int_Comp_Ref[0] = Record_Glob.Int_Comp;
        Proc_7(10, Int_Glob, Int_Comp_Ref);
        Record_Glob.Int_Comp = Int_Comp_Ref[0];

    }

    /* dhry.java method */
    void Proc_4() {

        boolean Bool_Loc;

        Bool_Loc = Char_Glob_1 == 'A';
        Bool_Loc = Bool_Loc || Bool_Glob;
        Char_Glob_2 = 'B';

    }

    /* dhry.java method */
    void Proc_5() {

        Char_Glob_1 = 'A';
        Bool_Glob = false;

    }

    /* dhry.java method */
    void Proc_6(int Enum_Par_Val, int Enum_Par_Ref[]) {

        Enum_Par_Ref[0] = Enum_Par_Val;

        if (!Func_3(Enum_Par_Val))
            Enum_Par_Ref[0] = Ident_4;

        switch (Enum_Par_Val) {

        case Ident_1:
            Enum_Par_Ref[0] = Ident_1;
            break;

        case Ident_2:
            if (Int_Glob > 100)
                Enum_Par_Ref[0] = Ident_1;
            else
                Enum_Par_Ref[0] = Ident_4;
            break;

        case Ident_3:
            Enum_Par_Ref[0] = Ident_2;
            break;

        case Ident_4:
            break;

        case Ident_5:
            Enum_Par_Ref[0] = Ident_3;
            break;

        }

    }

    /* dhry.java method */
    void Proc_7(int Int_Par_Val1, int Int_Par_Val2, int Int_Par_Ref[]) {

        int Int_Loc;

        Int_Loc        = Int_Par_Val1 + 2;
        Int_Par_Ref[0] = Int_Par_Val2 + Int_Loc;

    }

    /* dhry.java method */
    void Proc_8(int[] Array_Par_1_Ref, int[][] Array_Par_2_Ref, int Int_Par_Val_1, int Int_Par_Val_2) {

        int Int_Index,
            Int_Loc;

        Int_Loc = Int_Par_Val_1 + 5;
        Array_Par_1_Ref[Int_Loc] = Int_Par_Val_2;
        Array_Par_1_Ref[Int_Loc+1] = Array_Par_1_Ref[Int_Loc];
        Array_Par_1_Ref[Int_Loc+30] = Int_Loc;
        for (Int_Index = Int_Loc; Int_Index <= Int_Loc+1; ++Int_Index)
            Array_Par_2_Ref[Int_Loc][Int_Index] = Int_Loc;
        Array_Par_2_Ref[Int_Loc][Int_Loc-1] += 1;
        Array_Par_2_Ref[Int_Loc+20][Int_Loc] = Array_Par_1_Ref[Int_Loc];
        Int_Glob = 5;

    }

    /* dhry.java method */
    int Func_1(char Char_Par_1_Val, char Char_Par_2_Val) {

        char Char_Loc_1,
             Char_Loc_2;

        Char_Loc_1 = Char_Par_1_Val;
        Char_Loc_2 = Char_Loc_1;
        if (Char_Loc_2 != Char_Par_2_Val)
            return Ident_1;
        else
            return Ident_2;

    }

    /* dhry.java method */
    boolean Func_2(String String_Par_1_Ref, String String_Par_2_Ref) {

        int Int_Loc;
        char Char_Loc = '\0';

        Int_Loc = 2;
        while (Int_Loc <= 2)
            if (Func_1(String_Par_1_Ref.charAt(Int_Loc), String_Par_2_Ref.charAt(Int_Loc + 1)) == Ident_1) {
                Char_Loc = 'A';
                Int_Loc += 1;
            }
        if (Char_Loc >= 'W' && Char_Loc < 'Z')
            Int_Loc = 7;
        if (Char_Loc == 'X')
            return true;
        else {
            if (String_Par_1_Ref.compareTo(String_Par_2_Ref) > 0) {
                Int_Loc += 7;
                return true;
            } else
                return false;
        }

    }

    /* dhry.java method */
    boolean Func_3(int Enum_Par_Val) {

        int Enum_Loc;

        Enum_Loc = Enum_Par_Val;
        if (Enum_Loc == Ident_3)
            return true;
        else
            return false;

    }

    /* dhry.java method */
    public static void main(String argv[]) {
            //Msg_out = System.err;
    
            System.err.println("Dhrystone Benchmark, Version 2.1 (Language: Java)");
            System.err.println();
            //System.err.println("Please give the number of runs through the benchmark: ");
            dhry dh = new dhry();
            /*
            try {
                BufferedReader rdr
                    = new BufferedReader(new InputStreamReader(System.in));
                dh.Number_Of_Runs = Long.valueOf(rdr.readLine()).longValue();
            } catch (Exception e) {
                e.printStackTrace();
                return;
            }
            */
            dh.Number_Of_Runs = 10000000;
    
            dh.execute();
    }
}

