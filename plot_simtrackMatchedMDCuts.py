from __future__ import print_function
import plottery.plottery as ply
import ROOT as r
import numpy as np
import sys,os

#filename_PU200 = "debug_NuGun.root" #high pileup dude goes here
filename_PU200 = "../20191018_minidoublet_cut_investigations/debug_ttbar.root"
filename_muonGun = "debug_pt0p99_1p01.root"
if len(sys.argv) > 1:
    filename = sys.argv[1]


#Normalize the track matched muon gun histograms with respect to complete muon gun histogrmas (i.e., not track matched). This is to ensure the track matched distribution (which is intended to be a subset of the larger distribution) has a smaller integral than the PU200 distribution

def normalize_histograms(hist_PU200,hist_muonGun_matched,hist_muonGun_unmatched):
    hist_PU200.Scale(1.0/hist_PU200.Integral())
    hist_muonGun_matched.Scale(1.0/hist_muonGun_unmatched.Integral())
    hist_muonGun_unmatched.Scale(1.0/hist_muonGun_unmatched.Integral())

    return hist_PU200,hist_muonGun_matched,hist_muonGun_unmatched


def plot_representative_md_params(PU200_hist,muonGun_matched_hist,muonGun_hist,prefix,additional_options = None): 
    PU200_normed_hist, muonGun_matched_normed_hist, muonGun_normed_hist = normalize_histograms(PU200_hist,muonGun_matched_hist,muonGun_hist)
    #PU200_normed_hist,muonGun_matched_normed_hist,muonGun_normed_hist = (PU200_hist,muonGun_matched_hist,muonGun_hist)
    print("PU200 integral=",PU200_normed_hist.Integral())
    print("unmatched_integral=",muonGun_normed_hist.Integral())
    filename_prefix = prefix.replace(" ","_")
    filename_prefix = "/home/users/bsathian/public_html/SDL/SDL_Minidoublets/20191110_lowPt_muGunvttbar/"+filename_prefix+"_representative"
#     filename_prefix = "/home/users/bsathian/public_html/SDL/SDL_Minidoublets/20191017_NuGun_tenMuon/"+filename_prefix

    default_options = {
        "output_name":filename_prefix+".pdf",
        "do_stack":False,
        "legend_smart":False,
        "xaxis_label":"",
        "xaxis_range":[-10,10],
        "yaxis_log":True,
        "yaxis_range":[1e-6,1],
#        "yaxis_range":[0.1,1e6],
        "title":prefix,
        "legend_percentageinbox":False,
        }
    if "dPhi" in prefix and "dPhiChange" not in prefix:
       default_options["xaxis_range"] = [-0.5,0.5]
    options = default_options.copy()
    if type(additional_options) is dict:
        options.update(additional_options)

    ply.plot_hist(
        bgs = [PU200_normed_hist,muonGun_matched_normed_hist,muonGun_normed_hist],
        legend_labels = ["High pile-up","sim-hit matched muon gun","all muon-gun"],
        options = options)




def plot_md_params(PU200_hist,muonGun_matched_hist,muonGun_hist,prefix,additional_options = None):
    PU200_normed_hist, muonGun_matched_normed_hist, muonGun_normed_hist = normalize_histograms(PU200_hist,muonGun_matched_hist,muonGun_hist)
    filename_prefix = prefix.replace(" ","_")
    filename_prefix = "/home/users/bsathian/public_html/SDL/SDL_Minidoublets/20191110_lowPt_muGunvttbar/"+filename_prefix
#    filename_prefix = "/home/users/bsathian/public_html/SDL/SDL_Minidoublets/20191017_NuGun_tenMuon/"+filename_prefix

    default_options = {
        "output_name":filename_prefix+".pdf",
        "do_stack":False,
        "legend_smart":False,
        "xaxis_range":[-10,10],
        "yaxis_log":True,
        "yaxis_range":[1e-6,1],
#        "yaxis_range":[0.1,1e6],
        "xaxis_label":"",
        "title":prefix,
        "legend_percentageinbox":False,
        }
    if "dPhi" in prefix and "dPhiChange" not in prefix:
       default_options["xaxis_range"] = [-0.5,0.5]
    options = default_options.copy()
    if type(additional_options) is dict:
        options.update(additional_options)

    ply.plot_hist(
        bgs = [PU200_normed_hist,muonGun_matched_normed_hist],
        legend_labels = ["High pile-up","Matched muon gun"],
#        legend_labels = ["Rec hits","Rec hits matched to sim hits"],
        colors = [r.kBlue,r.kRed],
        options = options)


f_PU200 = r.TFile(filename_PU200)
f_muonGun = r.TFile(filename_muonGun)

dz_hist_PU200 = f_PU200.Get("Root__MD_dz")
dz_hist_muonGun = f_muonGun.Get("Root__MD_dz")
dz_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_dz")

dz_barrel_hist_PU200 = f_PU200.Get("Root__MD_barrel_dz")
dz_barrel_hist_muonGun = f_muonGun.Get("Root__MD_barrel_dz")
dz_barrel_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_barrel_dz")

dz_endcap_hist_PU200 = f_PU200.Get("Root__MD_endcap_dz")
dz_endcap_hist_muonGun = f_muonGun.Get("Root__MD_endcap_dz")
dz_endcap_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_endcap_dz")

dPhi_hist_PU200 = f_PU200.Get("Root__MD_dPhi")
dPhi_hist_muonGun = f_muonGun.Get("Root__MD_dPhi")
dPhi_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_dPhi")

dPhi_barrel_hist_PU200 = f_PU200.Get("Root__MD_barrel_dPhi")
dPhi_barrel_hist_muonGun = f_muonGun.Get("Root__MD_barrel_dPhi")
dPhi_barrel_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_barrel_dPhi")

dPhi_endcap_hist_PU200 = f_PU200.Get("Root__MD_endcap_dPhi")
dPhi_endcap_hist_muonGun = f_muonGun.Get("Root__MD_endcap_dPhi")
dPhi_endcap_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_endcap_dPhi")

dPhiChange_hist_PU200 = f_PU200.Get("Root__MD_dPhiChange")
dPhiChange_hist_muonGun = f_muonGun.Get("Root__MD_dPhiChange")
dPhiChange_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_dPhiChange")

dPhiChange_barrel_hist_PU200 = f_PU200.Get("Root__MD_barrel_dPhiChange")
dPhiChange_barrel_hist_muonGun = f_muonGun.Get("Root__MD_barrel_dPhiChange")
dPhiChange_barrel_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_barrel_dPhiChange")

dPhiChange_endcap_hist_PU200 = f_PU200.Get("Root__MD_endcap_dPhiChange")
dPhiChange_endcap_hist_muonGun = f_muonGun.Get("Root__MD_endcap_dPhiChange")
dPhiChange_endcap_hist_matched_muonGun = f_muonGun.Get("Root__sim_matched_MD_endcap_dPhiChange")


dz_layer_hists_PU200 = []
dz_layer_hists_muonGun = []
dz_layer_hists_matched_muonGun = []

dz_barrel_layer_hists_PU200 = []
dz_barrel_layer_hists_muonGun = []
dz_barrel_layer_hists_matched_muonGun = []

dz_endcap_layer_hists_PU200 = []
dz_endcap_layer_hists_muonGun = []
dz_endcap_layer_hists_matched_muonGun = []


dPhi_layer_hists_PU200 = []
dPhi_layer_hists_muonGun = []
dPhi_layer_hists_matched_muonGun = []

dPhi_barrel_layer_hists_PU200 = []
dPhi_barrel_layer_hists_muonGun = []
dPhi_barrel_layer_hists_matched_muonGun = []

dPhi_barrel_layer_center_hists_PU200 = []
dPhi_barrel_layer_center_hists_muonGun = []
dPhi_barrel_layer_center_hists_matched_muonGun = []

dPhi_barrel_layer_tilted_regular_hists_PU200 = []
dPhi_barrel_layer_tilted_regular_hists_muonGun = []
dPhi_barrel_layer_tilted_regular_hists_matched_muonGun = []

dPhi_barrel_layer_tilted_endcap_hists_PU200 = []
dPhi_barrel_layer_tilted_endcap_hists_muonGun = []
dPhi_barrel_layer_tilted_endcap_hists_matched_muonGun = []


dPhi_endcap_layer_hists_PU200 = []
dPhi_endcap_layer_hists_muonGun = []
dPhi_endcap_layer_hists_matched_muonGun = []

dPhiChange_layer_hists_PU200 = []
dPhiChange_layer_hists_muonGun = []
dPhiChange_layer_hists_matched_muonGun = []

dPhiChange_barrel_layer_hists_PU200 = []
dPhiChange_barrel_layer_hists_muonGun = []
dPhiChange_barrel_layer_hists_matched_muonGun = []

dPhiChange_barrel_layer_center_hists_PU200 = []
dPhiChange_barrel_layer_center_hists_muonGun = []
dPhiChange_barrel_layer_center_hists_matched_muonGun = []

dPhiChange_barrel_layer_tilted_regular_hists_PU200 = []
dPhiChange_barrel_layer_tilted_regular_hists_muonGun = []
dPhiChange_barrel_layer_tilted_regular_hists_matched_muonGun = []

dPhiChange_barrel_layer_tilted_endcap_hists_PU200 = []
dPhiChange_barrel_layer_tilted_endcap_hists_muonGun = []
dPhiChange_barrel_layer_tilted_endcap_hists_matched_muonGun = []


dPhiChange_endcap_layer_hists_PU200 = []
dPhiChange_endcap_layer_hists_muonGun = []
dPhiChange_endcap_layer_hists_matched_muonGun = []

for layer in range(1,7):
    dz_layer_hists_PU200.append(f_PU200.Get("Root__MD_barrel_dz_layer_"+str(layer)))
    dz_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_barrel_dz_layer_"+str(layer)))
    dz_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_barrel_dz_layer_"+str(layer)))

    dz_barrel_layer_hists_PU200.append(f_PU200.Get("Root__MD_barrel_dz_layer_"+str(layer)))
    dz_barrel_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_barrel_dz_layer_"+str(layer)))
    dz_barrel_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_barrel_dz_layer_"+str(layer)))

    dPhi_layer_hists_PU200.append(f_PU200.Get("Root__MD_dPhi_layer_"+str(layer)))
    dPhi_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_dPhi_layer_"+str(layer)))
    dPhi_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_dPhi_layer_"+str(layer)))

    dPhi_barrel_layer_hists_PU200.append(f_PU200.Get("Root__MD_barrel_dPhi_layer_"+str(layer)))
    dPhi_barrel_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_barrel_dPhi_layer_"+str(layer)))
    dPhi_barrel_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_barrel_dPhi_layer_"+str(layer)))

    dPhi_barrel_layer_center_hists_PU200.append(f_PU200.Get("Root__MD_barrel_center_dPhi_layer_"+str(layer)))
    dPhi_barrel_layer_center_hists_muonGun.append(f_muonGun.Get("Root__MD_barrel_center_dPhi_layer_"+str(layer)))
    dPhi_barrel_layer_center_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_barrel_center_dPhi_layer_"+str(layer)))

    dPhiChange_layer_hists_PU200.append(f_PU200.Get("Root__MD_dPhiChange_layer_"+str(layer)))
    dPhiChange_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_dPhiChange_layer_"+str(layer)))
    dPhiChange_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_dPhiChange_layer_"+str(layer)))

    dPhiChange_barrel_layer_hists_PU200.append(f_PU200.Get("Root__MD_barrel_dPhiChange_layer_"+str(layer)))
    dPhiChange_barrel_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_barrel_dPhiChange_layer_"+str(layer)))
    dPhiChange_barrel_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_barrel_dPhiChange_layer_"+str(layer)))

    dPhiChange_barrel_layer_center_hists_PU200.append(f_PU200.Get("Root__MD_barrel_center_dPhiChange_layer_"+str(layer)))
    dPhiChange_barrel_layer_center_hists_muonGun.append(f_muonGun.Get("Root__MD_barrel_center_dPhiChange_layer_"+str(layer)))
    dPhiChange_barrel_layer_center_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_barrel_center_dPhiChange_layer_"+str(layer)))



    if layer < 4:
        dPhi_barrel_layer_tilted_regular_hists_PU200.append(f_PU200.Get("Root__MD_dPhi_barrel_normal_tilted_layer_"+str(layer)))
        dPhi_barrel_layer_tilted_regular_hists_muonGun.append(f_muonGun.Get("Root__MD_dPhi_barrel_normal_tilted_layer_"+str(layer)))
        dPhi_barrel_layer_tilted_regular_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_dPhi_barrel_normal_tilted_layer_"+str(layer)))

        dPhi_barrel_layer_tilted_endcap_hists_PU200.append(f_PU200.Get("Root__MD_dPhi_barrel_endcapLogic_tilted_layer_"+str(layer)))
        dPhi_barrel_layer_tilted_endcap_hists_muonGun.append(f_muonGun.Get("Root__MD_dPhi_barrel_endcapLogic_tilted_layer_"+str(layer)))
        dPhi_barrel_layer_tilted_endcap_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_dPhi_barrel_endcapLogic_tilted_layer_"+str(layer)))

        dPhiChange_barrel_layer_tilted_regular_hists_PU200.append(f_PU200.Get("Root__MD_dPhiChange_barrel_normal_tilted_layer_"+str(layer)))
        dPhiChange_barrel_layer_tilted_regular_hists_muonGun.append(f_muonGun.Get("Root__MD_dPhiChange_barrel_normal_tilted_layer_"+str(layer)))
        dPhiChange_barrel_layer_tilted_regular_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_dPhiChange_barrel_normal_tilted_layer_"+str(layer)))

        dPhiChange_barrel_layer_tilted_endcap_hists_PU200.append(f_PU200.Get("Root__MD_dPhiChange_barrel_endcapLogic_tilted_layer_"+str(layer)))
        dPhiChange_barrel_layer_tilted_endcap_hists_muonGun.append(f_muonGun.Get("Root__MD_dPhiChange_barrel_endcapLogic_tilted_layer_"+str(layer)))
        dPhiChange_barrel_layer_tilted_endcap_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_dPhiChange_barrel_endcapLogic_tilted_layer_"+str(layer)))


    if layer < 6 :
        dz_endcap_layer_hists_PU200.append(f_PU200.Get("Root__MD_endcap_dz_layer_"+str(layer)))
        dz_endcap_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_endcap_dz_layer_"+str(layer)))
        dz_endcap_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_endcap_dz_layer_"+str(layer)))

        dPhi_endcap_layer_hists_PU200.append(f_PU200.Get("Root__MD_endcap_dPhi_layer_"+str(layer)))
        dPhi_endcap_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_endcap_dPhi_layer_"+str(layer)))
        dPhi_endcap_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_endcap_dPhi_layer_"+str(layer)))

        dPhiChange_endcap_layer_hists_PU200.append(f_PU200.Get("Root__MD_endcap_dPhiChange_layer_"+str(layer)))
        dPhiChange_endcap_layer_hists_muonGun.append(f_muonGun.Get("Root__MD_endcap_dPhiChange_layer_"+str(layer)))
        dPhiChange_endcap_layer_hists_matched_muonGun.append(f_muonGun.Get("Root__sim_matched_MD_endcap_dPhiChange_layer_"+str(layer)))




plot_md_params(dz_hist_PU200,dz_hist_matched_muonGun,dz_hist_muonGun,"Mini-doublet dz histogram")
plot_representative_md_params(dz_hist_PU200,dz_hist_matched_muonGun,dz_hist_muonGun,"Mini-doublet dz histogram")

plot_md_params(dz_barrel_hist_PU200,dz_barrel_hist_matched_muonGun,dz_barrel_hist_muonGun,"Mini-doublet dz histogram in barrel")
plot_md_params(dz_endcap_hist_PU200,dz_endcap_hist_matched_muonGun,dz_endcap_hist_muonGun,"Mini-doublet dz histogram in endcap")

plot_md_params(dPhi_hist_PU200,dPhi_hist_matched_muonGun,dPhi_hist_muonGun,"Mini-doublet dPhi histogram")
plot_md_params(dPhi_barrel_hist_PU200,dPhi_barrel_hist_matched_muonGun,dPhi_barrel_hist_muonGun,"Mini-doublet dPhi histogram in barrel")
plot_md_params(dPhi_endcap_hist_PU200,dPhi_endcap_hist_matched_muonGun,dPhi_barrel_hist_muonGun,"Mini-doublet dPhi histogram in endcap")

plot_md_params(dPhiChange_hist_PU200,dPhiChange_hist_matched_muonGun,dPhiChange_hist_muonGun,"Mini-doublet dPhiChange histogram")
plot_md_params(dPhiChange_barrel_hist_PU200,dPhiChange_barrel_hist_matched_muonGun,dPhiChange_barrel_hist_muonGun,"Mini-doublet dPhiChange histogram in barrel")
plot_md_params(dPhiChange_endcap_hist_PU200,dPhiChange_endcap_hist_matched_muonGun,dPhiChange_barrel_hist_muonGun,"Mini-doublet dPhiChange histogram in endcap")


for layer in range(1,7):
    plot_md_params(dz_layer_hists_PU200[layer-1],dz_layer_hists_matched_muonGun[layer-1],dz_layer_hists_muonGun[layer-1],"Mini-doublet dz histogram for layer "+str(layer))

    plot_md_params(dz_barrel_layer_hists_PU200[layer-1],dz_barrel_layer_hists_matched_muonGun[layer-1],dz_barrel_layer_hists_muonGun[layer-1],"Mini-doublet dz histogram in barrel for layer "+str(layer))

    plot_md_params(dPhi_layer_hists_PU200[layer-1],dPhi_layer_hists_matched_muonGun[layer-1],dPhi_layer_hists_muonGun[layer-1],"Mini-doublet dPhi histogram for layer "+str(layer))

    plot_md_params(dPhi_barrel_layer_hists_PU200[layer-1],dPhi_barrel_layer_hists_matched_muonGun[layer-1],dPhi_barrel_layer_hists_muonGun[layer-1],"Mini-doublet dPhi histogram in barrel for layer "+str(layer))

    plot_md_params(dPhiChange_layer_hists_PU200[layer-1],dPhiChange_layer_hists_matched_muonGun[layer-1],dPhiChange_layer_hists_muonGun[layer-1],"Mini-doublet dPhiChange histogram for layer "+str(layer))

    plot_md_params(dPhiChange_barrel_layer_hists_PU200[layer-1],dPhiChange_barrel_layer_hists_matched_muonGun[layer-1],dPhiChange_barrel_layer_hists_muonGun[layer-1],"Mini-doublet dPhiChange histogram in barrel for layer "+str(layer))



    if layer < 4:
        plot_md_params(dPhi_barrel_layer_center_hists_PU200[layer-1],dPhi_barrel_layer_center_hists_matched_muonGun[layer-1],dPhi_barrel_layer_center_hists_muonGun[layer-1],"Mini-doublet dPhi histogram for the center modules in barrel for layer "+str(layer))

        plot_md_params(dPhi_barrel_layer_tilted_regular_hists_PU200[layer-1],dPhi_barrel_layer_tilted_regular_hists_matched_muonGun[layer-1],dPhi_barrel_layer_tilted_regular_hists_muonGun[layer-1],"Mini-doublet dPhi histogram for regular tilted modules in barrel for layer "+str(layer))

        plot_md_params(dPhiChange_barrel_layer_center_hists_PU200[layer-1],dPhiChange_barrel_layer_center_hists_matched_muonGun[layer-1],dPhiChange_barrel_layer_center_hists_muonGun[layer-1],"Mini-doublet dPhiChange histogram for the center modules in barrel for layer "+str(layer))

        plot_md_params(dPhiChange_barrel_layer_tilted_regular_hists_PU200[layer-1],dPhiChange_barrel_layer_tilted_regular_hists_matched_muonGun[layer-1],dPhiChange_barrel_layer_tilted_regular_hists_muonGun[layer-1],"Mini-doublet dPhiChange histogram for regular tilted modules in barrel for layer "+str(layer))



    if layer < 3:
        plot_md_params(dPhi_barrel_layer_tilted_endcap_hists_PU200[layer-1],dPhi_barrel_layer_tilted_endcap_hists_matched_muonGun[layer-1],dPhi_barrel_layer_tilted_endcap_hists_muonGun[layer-1],"Mini-doublet dPhi histogram for endcap like tilted modules in barrel for layer "+str(layer))

        plot_md_params(dPhiChange_barrel_layer_tilted_endcap_hists_PU200[layer-1],dPhiChange_barrel_layer_tilted_endcap_hists_matched_muonGun[layer-1],dPhiChange_barrel_layer_tilted_endcap_hists_muonGun[layer-1],"Mini-doublet dPhiChange histogram for endcap like tilted modules in barrel for layer "+str(layer))


    if layer < 6:
        endcap_additional_options = {"xaxis_range":[-2,2]}
        plot_md_params(dz_endcap_layer_hists_PU200[layer-1],dz_endcap_layer_hists_matched_muonGun[layer-1],dz_endcap_layer_hists_muonGun[layer-1],"Mini-doublet dz histogram in endcap for layer "+str(layer),endcap_additional_options)

        plot_md_params(dPhi_endcap_layer_hists_PU200[layer-1],dPhi_endcap_layer_hists_matched_muonGun[layer-1],dPhi_endcap_layer_hists_muonGun[layer-1],"Mini-doublet dPhi histogram in endcap for layer "+str(layer))

        plot_md_params(dPhiChange_endcap_layer_hists_PU200[layer-1],dPhiChange_endcap_layer_hists_matched_muonGun[layer-1],dPhiChange_endcap_layer_hists_muonGun[layer-1],"Mini-doublet dPhiChange histogram in endcap for layer "+str(layer))





