using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace _10OcakRASAT
{
    internal static class Tester
    {
        public static void MainDegilim(String[] args)
        {
            
            GeneratorTData generator = new GeneratorTData();
            FileWriter fr = new FileWriter("TMUY2026_6334_TLM.csv");
            for (int i = 0; i < 10; i++)
            {
                fr.WriteData( generator.GenerateBasicTData());


            }
        }
    }
}
