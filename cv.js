function afficherCV() {
  const cv = document.getElementById("cv");
  cv.innerHTML = "";

  const sections = [
    { titre: "Education", contenu: "Parcous d'ingénieur niveau BAC+5 dans l'école polytechnique universitaire de Lille " },
    { titre: "Stages", contenu: "Stage de découvrte d'entreprise - Centre hospitalier du Calais" },
    { titre: "Certificats", contenu: "Coursera : Python for Data  /  Coursera : Excel/VBA pour la résolution créative des problèmes  /  Google : Data,Data,Data forever   /   EquityEdge : Qualified Funded Trader + FundedNext : Challenge Quant Analyst" },
    { titre: "Langages de programmation", contenu: "Python, C, R, SQL, PHP, HTML, CSS, Bootstrap, JavaScript" },
    { titre: "Outils de visualisation", contenu: "PowerBI, Tableau, Excel + VBA, Matlab" },
    { titre: "Systèmes et environnements", contenu: "Windows, Linux , Git" },
    { titre: "Loisirs", contenu: "Sport, jeux vidéo" }
  ];

  sections.forEach(sec => {
    const div = document.createElement("div");
    div.classList.add("mb-3");

    const h2 = document.createElement("h2");
    h2.textContent = sec.titre;

    const p = document.createElement("p");
    p.textContent = sec.contenu;

    div.appendChild(h2);
    div.appendChild(p);
    cv.appendChild(div);
  });
}

