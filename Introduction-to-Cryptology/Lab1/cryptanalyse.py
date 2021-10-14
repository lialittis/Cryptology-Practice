#!/usr/bin/env python3
##/users/morain/bin/python3
# author: Francois Morain (morain@lix.polytechnique.fr)
# modifications: Benjamin Smith (smith@lix.polytechnique.fr)
# version: 1.2.2  (several bugs corrected)
# version: 1.2.0  (new functionalities; one call is enough now; more error handing))
# version: 1.1.13 (enhanced modes)
# version: 1.1.12 (enhanced padawan mode)
# version: 1.1.11 (padawan mode)
# version: 1.1.10 (even better vowel algorithms; more user friendly)
# date: 2020/09/25

import datetime
import sys
import random

class Language:
    """data and tools for a given language"""
    def __init__(self, lg):
        if lg in ["fr", "en", "de", "es", "it"]:
            self.lg = lg
            self.nb_letters = 26
            self.upper_range = range(65, 91)
            self.lower_range = range(97, 123)
            self.vowels = ['a', 'e', 'i', 'o', 'u', 'y']
            self.freq = "./Data/"+lg+"/freq.data"
            self.bigr = "./Data/"+lg+"/bigrammes.data"
            self.dico = "./Data/"+lg+"/dico_"+lg+".data"
        else:
            print("Sorry, unknown language: "+lg)

def all_permutations(L):
    """ taken from http://tolokoban.org/Articles/EnumPerm/index.html"""
    n = len(L)
    if n == 1:
        return [L]
    # compute all permutations of n-1
    res = all_permutations(L[1:n])
    # add 0 where it should
    newres = []
    for s in res:
        for i in range(n):
            ss = s[0:i] + [L[0]] + s[i:n]
            newres.append(ss)
    return newres

def random_permutation(L):
    LL = L.copy()
    for i in range(0, len(L)-1):
        j = random.randint(i, len(L)-1)
        # swap
        tmp = LL[i]; LL[i] = LL[j]; LL[j] = tmp
    return LL

def load_freq(lg):
    """OUTPUT: freq[i] = frequency of chr(i)"""
    freq = {}
    with open(lg.freq, "r") as fd:
        for s in fd: # s = "a 0.00033"
            (a, f) = s.strip().split(' ')
            freq[ord(a)] = float(f)
    return freq

def load_bigr(lg):
    """OUTPUT: bigr[i, j]=freq of chr(i)chr(j)"""
    fd = open(lg.bigr, "r")
    bigr = {}
    trprob = {}
    with open(lg.bigr, "r") as fd:
        for s in fd:
            # s = "a ....."
            t = s.strip().split( )
            # t = ['a', '0.00033', ...]
            i = ord(t[0])
            tot = sum(float(x) for x in t[1:])
            for (k,tk) in enumerate(t[1:]): # presumably 26 letters...
                j = ord('a') + k
                bigr[i,j] = tk
                trprob[i,j] = float(tk)/tot
    return (bigr, trprob)

def load_prob(lg):
    freq = load_freq(lg)
    (bigr, trprob) = load_bigr(lg)
    return (freq, bigr, trprob)

def transition_prob(text, trprob):
    """sum of the transition probabilities for a partially decrypted text"""
    trps = [trprob[ord(x),ord(y)] 
            for (x,y) in zip(text[:-1],text[1:]) if(x != '-' and y != '-')]
    p = sum(trps)
    nz = sum(1 for trp in trps if trp == 0.0)
    return (p, nz)

def file_to_chartab(fic):
    """dump 'fic' into a char array with no '\n'"""
    with open(fic, "r") as f:
        return f.read( ).replace('\n','')

def contacts(lg, s):
    """OUTPUT: (l, lc, rc) where l = [(x, #lc[x] + #rc[x])] sorted w.r.t. .[1]
               lc[x] = left contacts of x
               rc[x] = right contacts of x
    """
    lcontact = {i:[] for i in lg.upper_range}
    rcontact = {i:[] for i in lg.upper_range}
    for (x,y) in zip(s[:-1],s[1:]): # pairs of adjacent letters xy in s:
        # x is a left contact for y, y is a right contact for x
        lcontact[ord(y)].append(ord(x))
        rcontact[ord(x)].append(ord(y))
        # TODO: case of doubled letters
    l = sorted(
            ((x, len(set(lcontact[x]).union(rcontact[x]))) for x in lg.upper_range),
            key=lambda c: -c[1])
    return (l, lcontact, rcontact)

def freqs(lg, s):
    """frequences (== int number of occurrences) for letters in s
    OUTPUT: (freq, bigr)
    """
    freq = {i:0 for i in lg.upper_range}
    bigr = {(i,j):0 for i in lg.upper_range for j in lg.upper_range}
    for x in s:
        freq[ord(x)] += 1
    for (x,y) in zip(s[:-1],s[1:]):
        bigr[ord(x),ord(y)] += 1
    return (freq, bigr)

def freq_tab(lg, s):
    """frequences for letters in s[]
    OUTPUT: (freq, bigr)
    """
    freq = {i:0 for i in lg.lower_range}
    for si in s:
        freq[ord(si)] += 1
    return freq

def multiple_patterns(fout, lg, s, l):
    """Find all patterns of length l and #occurrences > 1"""
    pat = {}
    for i in range(0, len(s)):
        key = s[i:i+l]
        if key in pat:
            pat[key] += 1
        else:
            pat[key] = 1
    ans = sorted(
        ((x, pat[x]) for x in pat.keys() if pat[x] > 1),
        key=lambda c:-c[1])
    return ans

def percent_to_string(per):
    return '{:03.2f}'.format(100*per)

def print_stats_raw(lg, fic, sfreq, bigr, N):
    """sfreq[0..26[ = [(x, freq[x])]"""
    for i in range(10):
        for (x,fx) in sfreq[i:i+20:10]:
            print("{0} {1} {2} ".format(chr(x), fx, percent_to_string((1.0*fx)/N)), end="")
        print()
    # get necessary number of spaces
    m = max(bigr.values( ))
    nsp = 0
    m10 = 1
    while m10 < m:
        m10 *= 10
        nsp += 1
    m10 /= 10
    nsp -= 1
    # print m, m10, nsp
    print(" "*nsp, end="")
    print(" ".join(" "*nsp + chr(x) for (x,fx) in sfreq))
    for (x,fx) in sfreq:
        print(chr(x) + " ", end="")
        for (y,fy) in sfreq:
            zz = z = bigr[x, y]
            if z == 0:
                z = 1
            while z < m10:
                print(" ", end="")
                z *= 10
            print("{0} ".format(str(zz) if zz != 0 else " "), end="")
        print()

########## html
def print_line_html(fout, ch, i, s):
    fout.write("<th>{0}</th><td>{1}</td><td>{2}</td>".format(ch,i,s))

def print_header_html(fout, fic):
    fout.write("""\
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN">
<html>
<head>
<title>Statistics for {0} </title>
<style type="text/css">
  th {{
    text-align: center;
    width: 30;
  }}
  td {{
    text-align: right;
    width: 30;
  }}
</style>
</head>
<body>
<h2>Statistics for {0}</h2>
""".format(fic))
            
def print_footer_html(fout):
    fout.write("""\
</body>
</html>
""")

def print_freq_html(fout, title, N, sfreq):
    """print_freq_html(fout, title, nb_of_letters, sorted frequencies[0..)
    where sfreq[i] = int = number of apparitions of chr(i).
    """
    fout.write('<table border="1">\n')
    fout.write(" <caption>{0}</caption>\n".format(title))
    for i in range(10):
        fout.write(" <tr>\n")
        for (x,fx) in sfreq[i:i+20:10]:
            per = percent_to_string((1.0*fx)/N)
            print_line_html(fout, chr(x), fx, per)
        fout.write('\n')
        fout.write(" </tr>\n")
    fout.write("</table>\n")

def print_bigr_html(fout, title, sfreq, bigr):
    """bigr[i][j] = number of bigrams chr(i)chr(j)"""
    fout.write("""\
<table border="1">
 <caption>{0}</caption>
 <tr>
""".format(title))
    # one empty cell
    fout.write('<td></td>')
    fout.write("".join("<th>{0}</th>".format(chr(x)) for (x,fx) in sfreq))
    fout.write(" </tr>\n")
    for (i,fi) in sfreq:
        fout.write(" <tr>\n")
        fout.write("<th>{0}</th>".format(chr(i)))
        fout.write("".join(
                    "<td>{0}</td>".format(bigr[i,j] if bigr[i,j] > 0 else "")
                    for (j,fj) in sfreq))
        fout.write(" </tr>\n")
    fout.write("</table>\n")
    # double letters
    fout.write("Double letters: ")
    fout.write("; ".join(
        "{0}{0}={1}".format(chr(i),bigr[i, i])
        for (i,fi) in sfreq if bigr[i,i] > 0))
    fout.write('\n')

def print_stats_html(lg, fic, sfreq, bigr, N, browser):
    """sfreq[0..26[ = [(x, freq[x])]"""
    (ref_freq, ref_bigr, trprob) = load_prob(lg)
    outname = "foo.html"
    with open(outname, "w") as fout:
        print_header_html(fout, fic)
        fout.write('Text with ' + str(N) + ' characters')
        # statistics
        fout.write("<table>\n");
        fout.write(" <td>\n");
        print_freq_html(fout, "frequencies", N, sfreq)
        fout.write(" </td>\n");
        # normalize ref_freq
        fout.write(" <td>\n");
        tmp_freq = sorted(
                ((x,int(N*ref_freq[x])) for x in ref_freq), 
                key=lambda c:-c[1])
        print_freq_html(fout, "reference (normalized)", N, tmp_freq)
        fout.write(" </td>\n");
        fout.write("</table>\n");
        # bigrams
        fout.write("<table>\n");
        fout.write(" <td>\n");
        print_bigr_html(fout, "bigrams", sfreq, bigr)
        fout.write(" </td>\n");
        fout.write(" <td>\n");
        tmp_bigr = {(i,j):int(N*float(ref_bigr[i, j])) 
                for i in lg.lower_range for j in lg.lower_range}
        print_bigr_html(fout, "reference (normalized)", tmp_freq, tmp_bigr)
        fout.write(" </td>\n");
        fout.write("</table>\n");
        # histograms
        fout.write("<pre>\n");
        for (x, fx) in sfreq:
            fout.write(chr(x)+" ");
            for i in range(fx//2):
                fout.write("*");
            fout.write("\n");
        fout.write("</pre>\n");
        fout.write("<pre>\n");
        for (x, fx) in tmp_freq:
            fout.write(chr(x)+" ");
            for i in range(fx//2):
                fout.write("*");
            fout.write("\n");
        fout.write("</pre>\n");
        # multiple patterns
        fout.write("<pre>\n");
        for l in range(3, len(msg)):
            mp = multiple_patterns(fout, lg, msg, l)
            if mp != []:
                fout.write("== Patterns of length "+str(l)+":\n");
                print(mp)
                for (pat, nb) in mp:
                    fout.write(pat+" "+str(nb)+" ");
                fout.write("\n");
            else:
                break
        fout.write("</pre>\n");
        print_footer_html(fout)
    if browser != "":
        import os
        os.system(browser+" "+outname)

########## tex
def print_stats_tex(lg, fic, sfreq, bigr, N):
    print("TEX: NYI")

########## stats
def print_stats(lg, fic, sfreq, bigr, N, opts):
    (form, browser, user, rd) = opts
    if form == "raw":
        print_stats_raw(lg, fic, sfreq, bigr, N)
    elif form == "html":
        print_stats_html(lg, fic, sfreq, bigr, N, browser)
    elif form == "tex":
        print_stats_tex(lg, fic, sfreq, bigr, N)

def treat_stats(lg, msg, opts):
    (form, browser, user, rd) = opts
    # frequencies and such
    (freq, bigr) = freqs(lg, msg)
    N = len(msg)
    sfreq = [(i,freq[i]) for i in sorted(lg.upper_range, key=lambda j:-freq[j])]
    if form != "none":
        print_stats(lg, fic, sfreq, bigr, N, opts)
    return (freq, bigr)

#################### monoalphabetic substitutions ####################        

# INPUT: msg is the ciphertext in A-Z
#        alph['A'] = 'e' yields the correspondance cypher -> clear
# Hence alph[msg[i]] = clear letter for msg[i].
def print_current(msg, alph, print_cipher):
    block = 50
    for i in range(0,len(msg),block):
        if print_cipher:
            print(msg[i:i+block])
        print("".join(alph[ord(x)] for x in msg[i:i+block]))

########## vowels vs. consonants
def sukhotin(lg, msg, freq, bigr, opt):
    """Sukhotin's method"""
    print('Sukhotin method')
    # contacts
    contacts = {(i,j):bigr[i,j]+bigr[j,i]
            for i in lg.upper_range for j in lg.upper_range}
    for i in lg.upper_range:
        contacts[i, i] = 0
    M = {i:sum(contacts[i,j] for j in lg.upper_range) for i in lg.upper_range}
    C = {i:True for i in lg.upper_range}
    lv = []
    while True:
        # find max
        m = 0
        for i in lg.upper_range:
            if C[i] and M[i] > m:
                v = i
                m = M[i]
        if m == 0:
            # no more vowel
            break
        lv.append(chr(v))
        C[v] = False
        # update data
        for i in (i for i in lg.upper_range if C[i]):
            M[i] -= 2*contacts[i, v]
        if len(lv) == 6:
            break
    print("Putative vowels:    " + " ".join(c for c in lv))
    return lv

def gaines(lg, msg, freq, opt):
    """the consonant-line method of Gaines"""
    print('Gaines method')
    # contacts
    (l, lc, rc) = contacts(lg, msg)
    # ready for establishing the line of consonants
    nbcontact = sum(x[1] for x in l)
    threshold = int(nbcontact * 0.8)
#    print("nbcontact=", nbcontact, "threshold=", threshold)
    nb = 0
    consonant = {i:False for i in lg.upper_range}
    s = {i:0 for i in lg.upper_range}
    for (x,ncx) in l:
        nb += ncx
        if nb >= threshold:
            consonant[x] = True
            for i in lc[x]:
                s[i] += 1
            for i in rc[x]:
                s[i] += 1
    # ready for first amplification: looking for letters not appearing in
    # any contact with the "sure-fire" consonants
    print("Suggested consonants: ", end="")
    for x in (x for (x,ncx) in l if s[x] == 0):
        print(chr(x) + " ", end="")
        consonant[x] = True
    print("\nRemaining letters:  ", end="")
    print(" ".join(chr(i) for i in sorted(
            (i for i in lg.upper_range if not consonant[i]),
            key=lambda j:-s[j])))

# ref_bigr[i, j] = str(float) for i, j in lower_range
def optimize_vowels(lg, msg, bigr, ref_bigr, lv):
    """try all permutations of vowels"""
    print("Minimal scores for all permutations of vowels")
    all = all_permutations([i for i in range(len(lv))])
    N = len(msg)
    # fbigr is in lowercase letters
    fbigr = {(i,j):float(bigr[ord(chr(i).upper()),ord(chr(j).upper())])/(N-1)
             for i in lg.lower_range for j in lg.lower_range}
    smin = 10000
    for perm in all:
        # map vperm to vowels
        vperm = [lv[x].lower() for x in perm]
        s = 0
        for i in range(len(vperm)):
            for j in range(len(vperm)):
                tmp = fbigr[ord(vperm[i]), ord(vperm[j])]
                tmp -= float(ref_bigr[ord(lg.vowels[i]), ord(lg.vowels[j])])
                s += tmp*tmp
        if s < smin:
            res = [[vperm[i].upper(),lg.vowels[i]] for i in range(len(vperm))]
            print(res)
            print(s)
            smin = s
    return res

def print_vowels(freq, bigr, lv, opts):
    ls = sorted(lv, key=lambda j:-freq[ord(j)])
    print("  ", end="")
    for v in ls:
        print(' {0:3}'.format(v), end="")
    print()
    for v in ls:
        print(v, end="")
        for w in ls:
            print('{0:3d}'.format(bigr[ord(v), ord(w)]), end=" ")
        print()

def optimize_consonnants(lg, msg, freq, alph):
    """OUTPUT: the alphabet maximizing the criterion"""
    print_alphabets(alph)
    print_current(msg, alph)
    consonnants = ['l', 'n', 't', 's', 'r']
    sfreq = [(i,freq[i]) for i in sorted(lg.upper_range, key=lambda j:-freq[j])]
    # find frequent letters that are not classified yet
    tbc = []
    for ifr in sfreq:
        (i, fr) = ifr
        if alph[i] == '-':
            tbc.append(i)
            if len(tbc) == len(consonnants):
                break
    print('Most frequent unclassified letters: ', end="")
    print([chr(i) for i in tbc])
    (ref_freq, ref_bigr, trprob) = load_prob(lg)
    smax = 0
    all = all_permutations([i for i in range(len(tbc))])
    for perm in all:
        for i in range(len(tbc)):
            alph[tbc[perm[i]]] = consonnants[i]
#            print(tbc[perm[i]], chr(tbc[perm[i]]),  consonnants[i])
        tt = msg
        for i in lg.upper_range:
#            print('Replacing', i, '=', chr(i), 'with', alph[i])
            tt = tt.replace(chr(i), alph[i])
        (p, nz) = transition_prob(tt, trprob)
        if p > smax:
            print("New transition proba=", p)
            smax = p
            alphmax = alph.copy()
            ttmax = tt
    print_alphabets(alphmax)
    print_current(msg, alphmax)
    return alphmax

def find_QU(lg, msg, freq, bigr, alph, opts):
    """In some languages, QU obeys the same rules"""
    if not lg.lg in ["fr", "es", "it"]:
        return []
    print('Trying to locate QU')
    print('candidateQ: [(candidateU), (candidateu), nb)]')
    sfreq = [(i,freq[i]) for i in sorted(lg.upper_range, key=lambda j:-freq[j])]
    for ifr in sfreq:
        (i, fr) = ifr
        if fr == 0:
            continue
        nj = 0
        li = []
        for j in lg.upper_range:
            if bigr[i, j] != 0:
                nj += 1
                li.append(j)
        if nj <= 2:
            print(chr(i), end=": ")
            print([(chr(x), alph[x], bigr[i, x]) for x in li])
    return []

# INPUT: ref_freq[97 = 'a'] is a float
#        freq[65 = 'A'] is an integer
# OUTPUT: alph[65 = 'A'] = 'z' and invalph['A'] = 1
def hill_init(lg, ref_freq, freq, opt):
    if opt == "plain":
        # be stupid for the first guess!
        alph = {i:chr(i).lower() for i in lg.upper_range}
    elif opt == "sorted":
        # sort both arrays
        sref = [i for i in sorted(lg.lower_range, key=lambda j:-ref_freq[j])]
        sfreq = [i for i in sorted(lg.upper_range, key=lambda j:-freq[j])]
        # force same ordering
        alph = {sfreq[i]:chr(sref[i]) for i in range(0, len(sref))}
        print(alph)
    elif opt == "random":
        L = random_permutation([i for i in lg.upper_range])
        print(L)
        alph = {L[i-lg.upper_range.start]:chr(i).lower() for i in lg.upper_range}
    return alph

# INPUT: ref_bigr_f[x, y]  for x, y in lower_range (float in [0, 1])
#        bigr_f[X, Y]      for x, y in upper_range (float in [0, 1])
#        alph[65 = 'A'] = 'z'
def hill_criterion(lg, ref_freq, ref_bigr_f, freq, bigr_f, alph):
    s = 0
    for X in lg.upper_range:
        # X and Y are cipher letters
        x = ord(alph[X])
        for Y in lg.upper_range:
            y = ord(alph[Y])
            tmp = ref_bigr_f[x, y]-bigr_f[X, Y]
#            print(X, Y, x, y, tmp)
            s += tmp*tmp
    return s

# INPUT: 0 <= a, b < #lg
# OUTPUT: two new distinct indices to be swapped
def hill_get_next_indices(lg, a, b, opt):
    if opt == "random":
        a = random.randint(0, len(lg.upper_range)-1)
        while True:
            b = random.randint(0, len(lg.upper_range)-1)
            if b != a:
                break;
    elif opt == "incr":
        a += 1
        if a+b >= len(lg.upper_range):
            a = 0
            b += 1
            if b >= len(lg.upper_range):
                b = 0 # trick!
    return (a, b)

# Based on Jakobsen95
# INPUT: msg in uppercase letters
#        ref_freq[97 = 'a'] is a float
#        freq[65 = 'A'] is an integer
#        ireg_bigr and bigr are integers[][]
def hill_climbing(lg, msg, ref_freq, ref_bigr_f, freq, bigr_f):
    print('entering hill_climbing')
#    print('msg='+msg)
    # alph{65...} = {lower_decoded letters}
    opt = "random"
    opt = "sorted"
    alph = hill_init(lg, ref_freq, freq, opt)
    print_current(msg, alph)
    # sfreq = array of chr's of letters by decreasing order of frequency
    # in the ciphertext
    sfreq = [i for i in sorted(lg.upper_range, key=lambda j:-freq[j])]
    # evaluate criterion
    k = hill_criterion(lg, ref_freq, ref_bigr_f, freq, bigr_f, alph)
    k2 = 2*k
    best = alph
    print('a0=', alph, '\nk0=', k)
    nb = 0
    # 0 <= a, a+b < size(alphabet) and refer to the ordering in sfreq
    # so that a=0 corresponds to the most frequent symbol in the ciphertext
    a = -1
    b = 1
    nt = 0
    found = False
    while True:
        alph = best.copy()
        (a, b) = hill_get_next_indices(lg, a, b, "incr")
        if b == 0:
            print('Best result for nt='+str(nt))
            print_current(msg, best)
            if nt == 5 or not found:
                break
            a = -1; b = 1; nt += 1; found = False;
        # exchange (a) and (b)
        #        print('exchaning '+str(a)+' and '+str(b))
        if False:
            tmp = alph[a]; alph[a] = alph[b]; alph[b] = tmp
        else:
#            print(a, b, a+b)
            aa = sfreq[a]
            bb = sfreq[a+b]
#            print(a, aa, alph[aa])
#            print(a+b, bb, alph[bb])
#            print('exchanging', alph[aa], 'and', alph[bb])
            tmp = alph[aa]; alph[aa] = alph[bb]; alph[bb] = tmp
        # evaluate new criterion
        newk = hill_criterion(lg, ref_freq, ref_bigr_f, freq, bigr_f, alph)
#        print_current(msg[0:20], alph)
#        print('newk=', newk)
#        sys.exit()
        # make evolve if needed
        if newk < k:
            print('newk_'+str(nb)+'='+str(newk))
            k = newk
            best = alph
            found = True
        elif newk < k2:
#            print('newk2_'+str(nb)+'='+str(newk))
            k2 = newk
    print_alphabets(best)
    print('best =', best)
    print('k =', k)
    sys.exit()

def subst_decrypt(lg, msg, opts):
    """OUTPUT: in padawan mode, returns a partial alphabet"""
    (form, stats, user, rd) = opts
    if user == "yoda":
        yoda_mode()
    # FIXME: clean this!
    (freq, bigr) = treat_stats(lg, msg, opts)
    (ref_freq, ref_bigr, trprob) = load_prob(lg)
    N = len(msg)
    # trial_begin
    bigr_f = {(i,j):float(bigr[ord(chr(i)),ord(chr(j))])/(N-1)
             for i in lg.upper_range for j in lg.upper_range}
    ref_bigr_f = {(i,j):float(ref_bigr[i,j])
             for i in lg.lower_range for j in lg.lower_range}
#    hill_climbing(lg, msg, ref_freq, ref_bigr_f, freq, bigr_f)
    # trial_end
    alph = {i:'-' for i in lg.upper_range}
    if user in ["padawan", "jedi"]:
        print('Looking for vowels')
        gaines(lg, msg, freq, form)
        lv = sukhotin(lg, msg, freq, bigr, form)
        print_vowels(freq, bigr, lv, opts)
        print('--------------------------')
        iref_bigr = {(i, j):int(N*float(ref_bigr[i, j]))
                     for i in lg.lower_range for j in lg.lower_range}
        print_vowels(ref_freq, iref_bigr, lg.vowels, opts)
        if user == "padawan":
            lv = optimize_vowels(lg, msg, bigr, ref_bigr, lv)
            # fill in vowels
            for vw in lv:
                (v, w) = vw
                alph[ord(v)] = w
            alph = optimize_consonnants(lg, msg, freq, alph)
            find_QU(lg, msg, freq, bigr, alph, opts)
    return alph

#################### interactive
def print_alphabets(alph):
    print("Cypher: " + "".join(chr(i) for i in lg.upper_range))
    print("Clear : " + "".join(alph[i] for i in lg.upper_range))

########## subst_loop
def subst_loop(lg, msg, alph, opts):
    help_str = """\
Help for the subst_loop
------------------
=Xx  replaces encrypted char 'X' by clear 'x'
=X-  undoes the substitution
D    displays alphabets and ciphertext/cleartext
C    displays the cleartext only    
U<user> change user mode
H    prints this help
Q    quits the loop
"""
    print_alphabets(alph)
    print_current(msg, alph, True)
    print("Type H for help; Q for quit")
    while True:
        try:
            cmd = input("subst> ")
        except EOFError:
            print("\nAu revoir")
            sys.exit( )
        if cmd == 'Q':
            break
        elif cmd == 'H':
            print(help_str)
        elif cmd == 'D':
            print_alphabets(alph)
            print_current(msg,alph,True)
        elif cmd == 'C':
            print_current(msg,alph,False)
        elif len(cmd) > 0 and cmd[0] == 'U':
            (output, browser, user) = opts
            if len(cmd) == 1:
                print('Current user is '+user)
            else:
                if valid_user(cmd[1:]):
                    # changing user
                    opts = (output, browser, cmd[1:])
                    alphnew = subst_decrypt(lg, msg, opts)
                    if len(alphnew) > 0:
                        alph = alphnew
        elif len(cmd) == 3 and cmd[0] == '=': # =Xx
            (enc,clear) = (cmd[1],cmd[2])
            if not ord(enc) in lg.upper_range:
                print("ERROR: {} not a valid encrypted letter".format(enc))
                continue
            if clear != '-' and not ord(clear) in lg.lower_range:
                print("ERROR: {} is not a valid clear letter".format(clear))
                continue
            alph[ord(enc)] = clear
            print_alphabets(alph)
            print_current(msg, alph, True)
        else:
            print("ERROR: unknown command '"+ cmd +"' (enter H for help)")

#################### probable words
# build the geometry of a string
def geometry(lg, w):
    g = []
    a = {i:0 for i in lg.upper_range}
    ind = 0
    for c in w:
        if a[ord(c)] == 0:
            ind += 1
            a[ord(c)] = ind
        g.append(a[ord(c)])
    return g

# finding all possible places for the given geometry
def find_probable_word(lg, msg, w):
    gw = geometry(lg, w.upper())
#    print("geom("+w+") =", gw
    return [ msg[i:i+len(w)] 
            for i in range(len(msg)-len(w)) 
            if geometry(lg, msg[i:i+len(w)]) == gw]

# w is in 'a-z'
def decrypt_alphabet(lg, w, cw):
    alph = {i:'-' for i in lg.upper_range}
    for (wi,cwi) in zip(w,cw):
        alph[ord(cwi)] = wi
    return alph

# enc(w) = li[0]
def use_probable_word(lg, msg, w, li):
    alph = decrypt_alphabet(lg, w, li)
    subst_loop(lg, msg, alph)

def plausibility(lg, lgfreq, lgbigr, tt):
    freq = {i:0.0 for i in lg.lower_range}
    for i in (ord(x) for x in tt if x != '-'):
        freq[i] += 1.0/len(tt)
    return sum(abs(freq[i]-lgfreq[i]) for i in lg.lower_range)

def use_dico(msg, lg, lgfreq, lgbigr, trprob, alph0):
    for k in range(9, 10):
        pmax = 0
        plausmin = 1000
        with open(lg.dico, "r") as fd:
            nw = 0
            for w in fd:
                w = w.strip()
                # stupid feature!
                if lg.lg == "fr":
                    w = w.replace(',', '').replace('"', '').replace(":", '')
                if len(w) != k:
                    continue
                nw += 1
                if nw % 1000 == 0:
                    print("# {0}-th word: {1} {2} {3}".format(nw,w,pmax,plausmin))
    #            print("# trying w=["+w+"]"
                li = find_probable_word(lg, msg, w)
                if len(li) == 1:
                    alph = decrypt_alphabet(lg, w, li[0])
                    # alph = {65: 's', 66: '-', 67: '-', 68: 'a', 69: '-', ...}
                    if all(alph[i] == '-' or alph0[i] == '-' or alph[i] == alph0[i]
                            for i in lg.upper_range):
                        tt = msg
                        for i in lg.upper_range:
                            tt = tt.replace(chr(i), alph[ord(msg[i])])
                        plaus = plausibility(lg, lgfreq, lgbigr, tt)
                        (p, nz) = transition_prob(tt, trprob)
                        if (nz <= 5 and p > 0.75*pmax) or (plaus < 1.25*plausmin):
                            print("w={0} -> {1} {2} {3} {4}".format(w,li[0],p,nz,plaus))
                            pmax = max(pmax,p)
                            plausmin = min(plausmin,plaus)

####################
def treat_substitution(lg, fic, msg, probable_word, key, opts):
    (form, browser, user, rd) = opts
    random.seed(int(rd))
    (lgfreq, lgbigr, trprob) = load_prob(lg)
    if dico:
        alph0 = {i:'-' for i in lg.upper_range}
        # TMP: 68 should be the most frequent letter
        alph0[68] = 'e'
        use_dico(msg, lg, lgfreq, lgbigr, trprob, alph0)
    elif probable_word != None:
        li = find_probable_word(lg, msg, probable_word)
        if len(li) == 1:
            print("We may try with", li[0])
            use_probable_word(lg, msg, probable_word, li[0])
    else:
        if key != None:
            alph = {i:key[i-65] for i in lg.upper_range}
            print("using key=", alph)
        else:
            alph = subst_decrypt(lg, msg, opts)
        if len(alph) == 0:
            alph = {i:'-' for i in lg.upper_range}
        subst_loop(lg, msg, alph, opts)

#################### Vigenere

def print_divisors(n):
    print(" ".join(str(d) for d in range(2, n+1) if n % d == 0))

def find_repeated_patterns(msg):
    for i in range(len(msg)):
        for j in range(i+4, len(msg)):
            # pat = msg[i..j]
            pat = msg[i:j]
            found = False
            for k in range(j+1, len(msg)-len(pat)):
#                print(msg[k:k+j-i]+" "+pat)
                if msg[k:k+j-i] == pat:
                    print("{0} i1={1} i2={2}; l={3} with divisors: ".format(pat,i,k,k-i), end="")
                    print_divisors(k-i)
                    found = True
            if not found:
                # if found = false, no match on i..j found => increase i
                break

def coincidence_index(msg):
    lci = [(i,sum(1 for (x,y) in zip(msg,msg[i:]+msg[:i]) if x == y))
            for i in range(1,50)]
    return sorted(lci,key=lambda c: -c[1])

def print_array(tab, K):
    nrows = len(tab) // K
    if len(tab) % K != 0:
        nrows = nrows+1
    for i in range(nrows):
        print(" ".join(str(x) for x in tab[i*K:(i+1)*K]))

def shift_letter(lg, ch, s):
    newch = ord(ch.lower())+s;
    if not newch in lg.lower_range:
        newch -= lg.nb_letters;
    return chr(newch)

def shift_column(clear, lg, msg, K, j, s):
    nrows = len(msg) // K
    if len(msg) % K != 0:
        nrows = nrows+1
    for i in range(nrows):
        jj = i*K+j
        if i < nrows-1 or jj < len(msg):
            clear[jj] = shift_letter(lg, msg[jj], s)
                
def shift_one_column(lg, msg, K, j, s):
    nrows = len(msg) // K
    if len(msg) % K != 0:
        nrows = nrows+1
    # determine real number of rows for column j
    if nrows*K+j >= len(msg):
        nrows -= 1
    return [shift_letter(lg, msg[i*K+j],s) for i in range(nrows)]

def most_frequent(lg, sfreq, cols):
    """intersect the letters in cols with the most frequent"""
    f = freq_tab(lg, cols)
    sf = sorted(lg.lower_range, key=lambda j:-f[j])
    return sum(1 for x in sf[0:10] if x in sfreq)
##    return sum(1 for (x,fx) in sf[0:10] if (x,fx) in sfreq)

# There are K columns and len(msg)//K rows.
def best_shift(clear, lg, msg, K, S, j):
    (lgfreq, lgbigr, trprob) = load_prob(lg)
    mostfreq = sorted(lg.lower_range, key=lambda i:-lgfreq[i])[0:10]
    tab = []
    for s in range(lg.nb_letters):
        cols = shift_one_column(lg, msg, K, j, s)
        nb = most_frequent(lg, mostfreq, cols)
        tr = 0
        # len(cols) is almost always len(msg)//K
        # see right of the column if j+1 was found
        if j < K-1 and S[j+1] != -1:
            tr += sum(trprob[ord(cols[i]), ord(clear[i*K+j+1])]
                      for i in range(len(cols)))
        # see left of the column if j-1 was found
        if j > 0 and S[j-1] != -1:
            tr += sum(trprob[ord(clear[i*K+j-1]), ord(cols[i])]
                    for i in range(len(cols)))
        tab.append([s, nb, int(100*tr)])
    print("[shift, #intersection, trans_proba]")
    print(sorted(tab,key=lambda x:-x[1])[0:5], sep='\n')

def reset_column(clear, lg, K, S, j):
    S[j] = -1
    for i in range(j,len(clear),K):
        clear[i] = '-'

def vigenere_loop(lg, msg, K):
    vigenere_loop_help = """\
Help for the vigenere_loop
--------------------------
Bj    finds the best shift for column j, 0 <= j < K
Sj.s  adds s to column j
Rj    resets column j
H     prints this help
Q     quits the loop"""
    print("Type H for help; Q for quit")
    clear = ['-' for i in range(len(msg))]
    S = [-1 for i in range(0, K)]
    print_array(clear, K)
    print(S)
    while True:
        try:
            cmd = input("vig> ")
        except EOFError:
            print("\nAu revoir")
            sys.exit( )
        if len(cmd) == 0:
            continue
        elif cmd == 'Q':
            break
        elif cmd == 'H':
            print(vigenere_loop_help)
        elif len(cmd) >= 2 and cmd[0] == 'B':
            if not str.isnumeric(cmd[1:]):
                print('Not a number '+cmd[1:])
                continue
            j = int(cmd[1:])
            if j <= K:
                best_shift(clear, lg, msg, K, S, j)
        elif len(cmd) >= 2 and cmd[0] == 'R':
            if not str.isnumeric(cmd[1:]):
                print('Not a number '+cmd[1:])
                continue
            j = int(cmd[1:])
            if j <= K:
                reset_column(clear, lg, K, S, j)
                print_array(clear, K)
        elif len(cmd) >= 4 and cmd[0] == 'S':
            # Sj.s shift colum j from s steps
            tmp = cmd[1:].split('.')
            if not str.isnumeric(tmp[0]) or not str.isnumeric(tmp[1]):
                print('I want integers: ', tmp[0], tmp[1])
                continue
            (j,s) = (int(tmp[0]),int(tmp[1]))
            if j <= K:
                S[j] = s
                shift_column(clear, lg, msg, K, j, s)
                print_array(clear, K)
        else:
            print("ERROR: unknown command '"+cmd+"' (enter H for help)")

def treat_vigenere(lg, fic, msg, probable_word, opts):
    print("# find repeated patterns: pattern i1 i2 len=i2-i1, divisors of len")
    find_repeated_patterns(msg)
    print("# compute coincidences: [(length, number of coincidences)]")
    l = coincidence_index(msg)
    print(l[1:10]) # FIXME: shouldn't this be l[:10] ??
    try:
        K = input("Key length? ")
    except EOFError:
        print("\nAu revoir")
        sys.exit( )
    vigenere_loop(lg, msg, int(K))

#################### main
def yoda_mode():
    print("May I suggest the force is enough for you to decrypt everything?")
    sys.exit()

def valid_user(user):
    users = ["padawan", "jedi", "master", "yoda"]
    b = user in users
    if not b:
        print('Valid users are ', end="")
        print(users);
    return b

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
            description="Basic classical cryptanalysis tools",
            epilog="Must be launched from the directory containing the Data/ directory")
    parser.add_argument(
            "filename", 
            help="The ciphertext to be analysed")
    parser.add_argument(
            "-T", "--cryptosystem",
            help="The cryptosystem under analysis",
            default="substitution",
            choices=["substitution","vigenere"])
    parser.add_argument(
            "-L", "--language", 
            help="Supposed plaintext language",
            default="fr",
            choices=["fr", "en", "de", "es", "it"])
    parser.add_argument(
            "--output", "-F",
            help="Output format",
            default="html",
            choices=["none", "raw","html"])
    parser.add_argument(
            "-b", "--browser",
            help="Browser for HTML output",
            default="firefox")
    parser.add_argument(
            "--probable",
            help="Use probable word to start decrypting")
    parser.add_argument(
            "--dico",
            help="Used in conjunction with --prob",
            action="store_true",
            default=False)
    parser.add_argument(
            "--statistics",
            help="Compute statistics on the ciphertext and exit",
            action="store_true",
            default=True) # better True?
    parser.add_argument(
            "--user", 
            help="Type of user (default is jedi)",
            default="jedi",
            choices=["padawan", "jedi", "master", "yoda"])
    parser.add_argument(
            "--key", 
            help="partial key for substitution")
    parser.add_argument(
            "--random", 
            default="42",
            help="random number for various algorithms")
    args = parser.parse_args( )
    msg = file_to_chartab(args.filename)
    lg = Language(args.language)
    dico = args.dico
    stats = args.statistics
    fic = args.filename
    probable_word = args.probable
    key = args.key
    opts = (args.output, args.browser, args.user, args.random)

    print('using random seed:', args.random)
    if args.cryptosystem == "substitution":
        treat_substitution(lg, fic, msg, probable_word, key, opts)
    elif args.cryptosystem == "vigenere":
        treat_vigenere(lg, fic, msg, probable_word, opts)
