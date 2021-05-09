// cheaper version of a cos() or sin() wave
//plot (x/t%1) * (1-x/t%1) * h * 4 * (-1)^floor(x) with t = 1 and h = 7
// https://www.wolframalpha.com/input/?i=plot+%28x%2Ft%251%29+*+%281-x%2Ft%251%29+*+h+*+4+*+%28-1%29%5Efloor%28x%29+with+t+%3D+1+and+h+%3D+7
    float CheapCurve(float time, float LoopTime = 1.0f, float CurveHeight = 1.0f, bool Negative = false)
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

        float temp = (time / LoopTime % 1) * (1 - time / LoopTime % 1) * CurveHeight * 4;

        if (Negative)
        {
            temp *= Mathf.Pow(-1, Mathf.Floor(time));
        }

        return temp;
    }