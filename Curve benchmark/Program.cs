using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BenchmarkDotNet.Attributes;
using BenchmarkDotNet.Running;

namespace ConsoleApp1
{
    public class BenchmarkTest
    {

        private readonly double time;

        public BenchmarkTest()
        {
            time = new Random(69).NextDouble();
            time *= new Random(45).Next();
        }


        // cheaper version of a cos() or sin() wave
        [Benchmark]
        public double CheapCurve()
        {
            //old
            //per second interval plot (2x%2) * ((-2x)%2) * (1)^floor(x)
            //cheaper version (per 2 seconds): plot (x%2) * ((-x)%2) * (1)^floor(x)
            //c# % doesn't support inverse modulo, so here's a version without (per 1 second)
            //(2x%2) * (2-(2x%2))

            //float a = 2; // "speed", how fast we increment, simple terms the height
            //with a = 2, we'll get a 0 to 1 loop no matter what, then b is just a time coefficient
            //float b = Mathf.Pow((loopTime / a),-1); 
            // a / b = time
            // a/b = (b / a) ^ -1
            //so (time / a) ^ -1


            //new
            //(x/t%1) * (1-x/t%1) * h * 4
            //x(1−x) is symmetric around x=1/2, and that's where the peak is, so the peak height is 1/2 × 1/2 = 1/4.

            double LoopTime = 1.0f;
            double CurveHeight = 1.0f;
            bool Negative = false;

            double temp = (time / LoopTime % 1) * (1 - time / LoopTime % 1) * CurveHeight * 4;

            if (Negative)
            {
                temp *= Math.Pow(-1, Math.Floor(time));
            }

            return temp;
        }

        [Benchmark]
        public double CosCurve()
        {
            double LoopTime = 1.0f;
            double CurveHeight = 1.0f;
            bool Negative = false;


            double temp = Math.Cos(time / (LoopTime / Math.PI)) * CurveHeight;

            if (!Negative)
            {
                temp = Math.Abs(temp);
            }

            return temp;
        }
    }

    class Program
    {
        static void Main(string[] args)
        {

            var summary = BenchmarkRunner.Run<BenchmarkTest>();

            Console.ReadLine();
        }
    }
}
