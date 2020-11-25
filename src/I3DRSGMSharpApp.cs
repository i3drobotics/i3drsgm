using System;
using System.IO;
using I3DRSGMSharp;

namespace I3DRSGMSharpApp {
    static class i3drsgmSharpApp
    {
        static void Main(string[] args)
        {
            string cwd = Directory.GetCurrentDirectory();
            string resource_folder = cwd+"/resources";
            i3drsgmSharp i3drsgm = new i3drsgmSharp(resource_folder+"/left.yaml",resource_folder+"/right.yaml",resource_folder+"/output");
            bool valid = true;
            while(valid){
                valid = i3drsgm.forwardMatchFiles(resource_folder+"/left.png",resource_folder+"/right.png");
            }
            i3drsgm.close();
        }
    }
}