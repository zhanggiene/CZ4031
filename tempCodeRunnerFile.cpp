                bool rightSibHasRightSibl = hasRightSibling(parentNode, currentNode);
                if (rightSibHasRightSibl){
                    Node * rightRightSibling = (Node *)parentNode->children[indexOfRightSibling];
                    fixIndexes(parentNode, rightRightSibling);
                    return currentNode;
                } else {
                    return currentNode;
                }