y ++){
            for(int x = 0; x < piWidth; x++){
                Byte g = get_green(idata, x, y, width, size);
                Byte b = get_blue(idata, x, y, width, size);
                Byte r = get_red(idata, x, y,width, size);

                g += (int)(brightness * 255);
                b += (int) (brightness * 255);
                r += (int) (brightness * 255);

                g = min(g, (Byte) 255);
                b = min(b, (Byte) 255);
                r = min(r, (Byte) 255);

                p[(x*3)+0 + y * width] = g;
                p[(x*3)+1 + y * width] = b ;
                p[(x*3)+2 + y * width] = r;


            }
        }