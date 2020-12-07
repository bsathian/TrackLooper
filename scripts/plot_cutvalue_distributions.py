import uproot4
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
import awkward1 as ak
import mplhep
import sys
from coffea import hist
plt.style.use(mplhep.style.CMS)

f = uproot4.open(sys.argv[1])
tree = f["tree"]

def plot_distributions(obj):
    global tree
    blacklist = ["hitIdx","simTrkIdx","layer","pt","eta","phi","sim_pt","sim_eta","sim_phi","type"]
    print("object = ",obj)
    quantities = []
    for name in tree.keys():
        if name[:len(obj)] == obj and name not in map(lambda x : "{}_{}".format(obj,x),blacklist):
            quantities.append(name)

    a = tree["sim_{}Idx_isMTVmatch".format(obj)].array()
    b = ak.flatten(a, axis = 2)

    layers = tree["{}_layer".format(obj)].array()
    cats = []
    for event in layers:
        cats.append([])
        for trackObject in event:
            string = ""
            for layer in trackObject:
                if layer == 0:
                    string += "P"
                elif layer <= 6:
                    string += "B"
                else:
                    string += "E"
            cats[-1].extend([string])

    cats = ak.from_regular(cats)

    for quantity in quantities:
        print("quantity = ",quantity)
        qArray = tree[quantity].array()
        qArraySimTrackMatched = qArray[b]
        if all(ak.flatten(qArray) == -999):
            continue
        minValue = min(ak.flatten(qArray)[ak.flatten(qArray) > -999])
        maxValue = max(ak.flatten(qArray))
        histMinLimit = minValue * 1.5 if minValue < 0 else minValue * 0.75
        histMaxLimit = maxValue * 0.75 if maxValue < 0 else maxValue * 1.5
        print("minValue = {}, maxValue = {}".format(minValue,maxValue))

        allHist = hist.Hist("Events",hist.Bin("allValues","{}".format(quantity.split("_")[1]),1000,histMinLimit,histMaxLimit))
        simTrackMatchedHist = hist.Hist("Events",hist.Bin("simtrkMatched","Sim track Matched {}".format(quantity.split("_")[1]),1000,histMinLimit,histMaxLimit))

        allHist.fill(allValues = ak.flatten(qArray)[ak.flatten(qArray) > -999])
        simTrackMatchedHist.fill(simtrkMatched = ak.flatten(qArraySimTrackMatched)[ak.flatten(qArraySimTrackMatched) > -999])

        plt.figure()
        plt.yscale("log")
        ax = hist.plot1d(allHist,fill_opts = {
            'alpha': 1.0,
            'edgecolor':(0,0,0,.5),
            "color":"blue"}, )
        ax.set_label(quantity)
        hist.plot1d(simTrackMatchedHist,fill_opts = {
        'alpha': 0.5,
        'edgecolor':(0,0,0,.5),
        'color':'red'}, legend_opts = {"labels":[quantity, "sim track matched "+quantity]})
        plt.title("Distribution for {}".format(quantity))
        plt.savefig("{}.pdf".format(quantity))
        plt.close()


objects = ["md","sg","qp"]#,"psg","pqp","tp"]
[plot_distributions(i) for i in objects]
