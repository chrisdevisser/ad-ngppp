function getExtraRepl(replicanti){replicanti = replicanti || player.replicanti.amount.e; let ts225Eff = 0
    let ts226Eff = 0
    let speed = getQCReward(8) * 2
    if (player.timestudy.studies.includes(225)) {
        ts225Eff = Math.floor(replicanti / 1e3)
        if (ts225Eff > 99) ts225Eff = Math.floor(Math.sqrt(0.25 + (ts225Eff - 99) * speed) + 98.5)
    }
    if (player.timestudy.studies.includes(226)) {
        ts226Eff = Math.floor(player.replicanti.gal / 15)
        if (ts226Eff > 99) ts226Eff = Math.floor(Math.sqrt(0.25 + (ts226Eff - 99) * speed) + 98.5)
    }
let total = ts225Eff+ts226Eff;
if (total > 325) total = (Math.sqrt(0.9216+0.16*(total-324))-0.96)/0.08+324
if (tmp.ngp3) {
        gatheredQuarksBoost = Math.pow(tmp.qu.replicants.quarks.add(1).log10(),player.masterystudies.includes("t362")?0.35:0.25)*0.67*(player.masterystudies.includes("t412")?1.25:1)*(player.ghostify.ghostlyPhotons.unl?tmp.le[3]:1)
        total *= colorBoosts.g + gatheredQuarksBoost
    }
return total;}

function nextRepl() {
let current = getExtraRepl();
for (var repl = Math.floor(player.replicanti.amount.e/1000)*1000; getExtraRepl(repl) === current; repl += 1000);
return repl;
}