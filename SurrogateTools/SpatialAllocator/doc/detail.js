// JavaScript Document
 <script>
  function showDetail(evt)
  {
    var cellNode = getEvtTarget(evt);
    if (cellNode.tagName.toLowerCase() != "td") return;
    deselectExisting(document.getElementById("tabs"), "content");
    selectNew(cellNode, "content");
  }
  </script>