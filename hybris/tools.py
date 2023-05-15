from itertools import product, combinations

def prototypes_sets(max_value, order=2, return_associations=False):
    values_list = list(range(max_value))
    masks = []
    assoc = list(combinations(values_list, 2))
    assoc.extend([(a,a) for a in range(7)])
    for c in assoc:
        tmp = [0]*7
        for i in c:
            tmp[i] = 1
        masks.append("".join(map(str, tmp)))

    if return_associations:
        print(assoc)
        return masks, assoc
    else:
        return masks