/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape Corp.
 * Portions created by Netscape Corp.are Copyright (C) 2003 Netscape
 * Corp. All Rights Reserved.
 *
 * Original Author: Aaron Leventhal
 *
 * Contributor(s):
 *
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsXULTreeAccessibleWrap.h"
#include "nsTextFormatter.h"
#include "nsIFrame.h"

// --------------------------------------------------------
// nsXULTreeAccessibleWrap
// --------------------------------------------------------

nsXULTreeAccessibleWrap::nsXULTreeAccessibleWrap(nsIDOMNode *aDOMNode, nsIWeakReference *aShell):
nsXULTreeAccessible(aDOMNode, aShell)
{
}

NS_IMETHODIMP nsXULTreeAccessibleWrap::GetRole(PRUint32 *aRole)
{
  NS_ASSERTION(mTree, "No tree view");

  nsCOMPtr<nsITreeColumns> cols;
  mTree->GetColumns(getter_AddRefs(cols));
  nsCOMPtr<nsITreeColumn> primaryCol;
  if (cols) {
    cols->GetPrimaryColumn(getter_AddRefs(primaryCol));
  }
  // No primary column means we're in a list
  // In fact, history and mail turn off the primary flag when switching to a flat view
  *aRole = primaryCol ? ROLE_OUTLINE : ROLE_LIST;

  return NS_OK;
}

// --------------------------------------------------------
// nsXULTreeitemAccessibleWrap Accessible
// --------------------------------------------------------

nsXULTreeitemAccessibleWrap::nsXULTreeitemAccessibleWrap(nsIAccessible *aParent, 
                                                         nsIDOMNode *aDOMNode, 
                                                         nsIWeakReference *aShell, 
                                                         PRInt32 aRow, 
                                                         nsITreeColumn* aColumn) :
nsXULTreeitemAccessible(aParent, aDOMNode, aShell, aRow, aColumn)
{
}

NS_IMETHODIMP nsXULTreeitemAccessibleWrap::GetRole(PRUint32 *aRole)
{
  // No primary column means we're in a list
  // In fact, history and mail turn off the primary flag when switching to a flat view
  NS_ASSERTION(mColumn, "mColumn is null");
  PRBool isPrimary = PR_FALSE;
  mColumn->GetPrimary(&isPrimary);
  *aRole = isPrimary ? ROLE_OUTLINEITEM : ROLE_LISTITEM;
  return NS_OK;
}

NS_IMETHODIMP nsXULTreeitemAccessibleWrap::GetBounds(PRInt32 *x, PRInt32 *y, PRInt32 *width, PRInt32 *height)
{
  nsresult rv = nsXULTreeitemAccessible::GetBounds(x, y, width, height);
  if (NS_FAILED(rv)) {
    return rv;
  }
  nsIFrame *frame = GetFrame();
  if (frame) {
    // Will subtract first cell's start x from total width
    PRInt32 cellStartX, cellStartY;
    mTree->GetCoordsForCellItem(mRow, mColumn, EmptyCString(), &cellStartX, &cellStartY, width, height);
    // Use entire row width, not just key column's width
    float t2p = GetPresContext()->TwipsToPixels();
    *width = NSTwipsToIntPixels(frame->GetRect().width, t2p) - cellStartX;
  }
  return NS_OK;
}

NS_IMETHODIMP nsXULTreeitemAccessibleWrap::GetName(nsAString& aName)
{
  nsCOMPtr<nsITreeColumns> cols;
  mTree->GetColumns(getter_AddRefs(cols));
  if (!cols) {
    return NS_OK;
  }
  nsCOMPtr<nsITreeColumn> column;
  cols->GetFirstColumn(getter_AddRefs(column));
  while (column) {
    nsAutoString colText;
    mTreeView->GetCellText(mRow, column, colText);
    aName += colText + NS_LITERAL_STRING("  ");
    nsCOMPtr<nsITreeColumn> nextColumn;
    column->GetNext(getter_AddRefs(nextColumn));
    column = nextColumn;
  }
  return NS_OK;
}

NS_IMETHODIMP nsXULTreeitemAccessibleWrap::GetDescription(nsAString& aDescription)
{
  if (!mParent || !mWeakShell || !mTreeView) {
    return NS_ERROR_FAILURE;
  }

  PRUint32 itemRole;
  GetRole(&itemRole);
  if (itemRole == ROLE_LISTITEM) {
    return nsAccessibleWrap::GetDescription(aDescription);
  }

  aDescription.Truncate();

  PRInt32 level;
  if (NS_FAILED(mTreeView->GetLevel(mRow, &level))) {
    return NS_OK;
  }

  PRInt32 testRow = -1;
  if (level > 0) {
    mTreeView->GetParentIndex(mRow, &testRow);
  }

  PRInt32 numRows;
  mTreeView->GetRowCount(&numRows);

  PRInt32 indexInParent = 0, numSiblings = 0;

  // Get the index in parent and number of siblings
  while (++ testRow < numRows) {
    PRInt32 testLevel = 0;
    mTreeView->GetLevel(testRow, &testLevel);
    if (testLevel == level) {
      if (testRow <= mRow) {
        ++indexInParent;
      }
      ++numSiblings;
    }
    else if (testLevel < level) {
      break;
    }
  }

  // Count the number of children
  testRow = mRow;
  PRInt32 numChildren = 0;
  while (++ testRow < numRows) {
    PRInt32 testLevel = 0;
    mTreeView->GetLevel(testRow, &testLevel);
    if (testLevel <= level) {
      break;
    }
    else if (testLevel == level + 1) {
      ++ numChildren;
    }
  }

  // Don't localize "of" or "with" -- that is just the format of
  // the string, and are parsed out by the AT
  nsTextFormatter::ssprintf(aDescription, NS_LITERAL_STRING("L%d, %d of %d with %d").get(),
                            level + 1, indexInParent, numSiblings, numChildren);

  return NS_OK;
}

