import java.io.*;
import java.util.*;

public class Tema2 {
    public static void main(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }

        System.out.println(args[0]);
        System.out.println(args[1]);
        System.out.println(args[2]);
    }
}
