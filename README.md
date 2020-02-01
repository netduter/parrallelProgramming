<h1>Εργασία στο μάθημα παράλληλοι υπολογιστές</h1>

<h3>Άσκηση 1</h3>

<h5>Η άσκηση 1 έχει υλοποιηθεί σε cε C και σε C++</h5>

<p>H υλοποίηση με C bρίσκεται στον φάκελο <strong>Exercise1Cimplementation</strong>
Ενώ η υλοποίηση με c++ στον φάκελο <strong>Excersise1C++implementation</strong></br> </p>

<p>Για να γίνει compile ο κώδικας σε C++ απαιτείται το -std=c++11 flag. πχ. </p>

```bash

nikos@nikos-Inspiron-5559 ~/Έγγραφα/Codeblocks/mpi/Excersise1C++implementation $ g++ -std=c++11 main.cpp Exersise1.cpp -o main
nikos@nikos-Inspiron-5559 ~/Έγγραφα/Codeblocks/mpi/Excersise1C++implementation $ ./main ./../100.fastq results.txt
```
<p>Από την ασυμπτωτική ανάλυση περιμένουμε πως όταν ένας αλγόριθμος έχει υλοποιηθεί σε διαφορετικές γλώσσες ή 
και όταν εκτελείται σε διαφορετικά μηχανήματα ο χρόνος εκτέλεσης θα πρέπει να διαφέρει κατά μια σταθερά.</p>


 <img src="https://raw.githubusercontent.com/netduter/parrallelProgramming/master/Excersise1C++implementation/images/cVsCpp.png" style="margin-left=100px;" alt="Smiley face" height="500px" width="500px" > 

<p>Ωστόσο τα δυο διαγράμματα δεν είναι παράλληλα. Κάτι που αποτελεί ένδειξη πως τα δυο προγράμματα <strong>δεν</strong>     υλοποιούν τον ίδιο αλγόριθμο. </br> Πιθανότητα η μειωμένη απόδοση οφείλεται στο γεγονός πως το πρόγραμμα σε C++ ψάχνει για regular expressions διαβάζοντας από το αρχείο. Κάτι που έχει αυξημένη πολυπλοκότητα.</br> Στα δυο προγράμματα το throuput σταθεροποιείται από μια ορισμένη τιμή και μετά, κάτι που υποδεικνύει πως αν θέλουμε περισσότερη απόδοση θα πρέπει να περάσουμε σε παράλληλο προγραμματισμό. </p>

 <img src="https://raw.githubusercontent.com/netduter/parrallelProgramming/master/Excersise1C++implementation/images/cVsCppThrouput.png" style="margin-left=100px;" alt="Smiley face" height="500px" width="500px" > 
