/*
 * Tree View Node 
 *
 */
function TreeViewNode(me) {
    this.domElement = null;
    this.modelEntry = me;
}

/*
 * ElementViewNode
 *
 */
function ElementViewNode(me) {
    TreeViewNode.call(this, me);
}

//
// createDOMElement - creates a DOM element to make the section visible
//
ElementViewNode.prototype.createDOMElement = function() {
    // Create the DOM element
    var result = document.createElement("LI");
    result.appendChild(document.createTextNode(this.modelEntry.title));
    
    // Connect references for later use
    this.domElement = result;
    result.viewNode = this;

    return result;
};

/*
 * SectionViewNode
 *
 */
function SectionViewNode(me) {
    TreeViewNode.call(this, me);
    this.ulContainer = null;
    this.isExpanded = false;
}

//
// expand - Expands the visible list
//
SectionViewNode.prototype.expand = function() {
    var node;
    for(var i = 0; i < this.modelEntry.elements.length; i++) {
        if(this.modelEntry.elements[i].type == "section") {
            node = new SectionViewNode(this.modelEntry.elements[i]);
        }
        else if(this.modelEntry.elements[i].type == "element") {
            node = new ElementViewNode(this.modelEntry.elements[i]);
        }
        this.ulContainer.appendChild(node.createDOMElement());
    }
    this.isExpanded = true;
    this.pmContainer.innerHTML = " - ";
};

// 
// collapse - Collapses the visible list
//
SectionViewNode.prototype.collapse = function() {
    while(this.ulContainer.firstChild)
        this.ulContainer.removeChild(this.ulContainer.firstChild);
    this.isExpanded = false;
    this.pmContainer.innerHTML = " + ";
};
//
// createDOMElement - creates a DOM element to make the section visible
//
SectionViewNode.prototype.createDOMElement = function() {
    // Create the DOM element
    var result = document.createElement("LI");
    
    // Create the plus/minus container
    var pmDiv = document.createElement("DIV");
    pmDiv.appendChild(document.createTextNode(" + "));
    pmDiv.style.cssFloat = "left";
    pmDiv.style.cursor = "default";
    // When clicking the pm div => collapse/expand the list
    pmDiv.onclick = function() {
        if(this.parentNode.viewNode.isExpanded)
            this.parentNode.viewNode.collapse();
        else 
            this.parentNode.viewNode.expand();
    };
    
    // Create the title container
    var titleDiv = document.createElement("DIV");
    titleDiv.style.cursor = "default";
    titleDiv.appendChild(document.createTextNode(this.modelEntry.title));

    // Create the element container
    var ulDiv = document.createElement("UL");
    ulDiv.style.listStyleType = "none";
    
    // Append the containers to the main DOM element
    result.appendChild(pmDiv);
    result.appendChild(titleDiv);
    result.appendChild(ulDiv);

    // Connect references for later use
    this.domElement = result;
    this.ulContainer = ulDiv;
    this.pmContainer = pmDiv;
    result.viewNode = this;
    
    return result;
};

/*
 * TreeView 
 *
 */
function TreeView(model, container) {
   this.container = container;
   this.model = model;
}

//
// populateList - populate the visual list with elements
//
TreeView.prototype.populateList = function() {
    var rootNode = new SectionViewNode(this.model);
    this.container.appendChild(rootNode.createDOMElement());
};

