import React from 'react';
import {
  Box,
  Paper,
  Typography,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableRow,
  CircularProgress,
  Card,
  CardMedia,
  CardContent
} from '@mui/material';

export default function ProductViewer({ product, productPath }) {
  console.log('ProductViewer props:', { product, productPath });

  if (!product) {
    console.log('No product data available');
    return (
      <Box sx={{ display: 'flex', justifyContent: 'center', p: 3 }}>
        <CircularProgress />
      </Box>
    );
  }

  if (product.type === 'error') {
    console.log('Product error:', product.error);
    return (
      <Paper sx={{ p: 2, textAlign: 'center' }}>
        <Typography color="error">
          {product.error || 'Failed to parse product data'}
        </Typography>
      </Paper>
    );
  }

  console.log('Rendering product data:', product);

  // Function to render a specific field based on its key and value
  const renderField = (key, value) => {
    // Skip rendering these fields as they are handled separately
    if (key === 'id') return null;
    
    // Handle special case for data field that might contain image data
    if (key === 'data' && typeof value === 'string' && value.startsWith('data:image/')) {
      return (
        <TableRow key={key}>
          <TableCell component="th" scope="row" colSpan={2}>
            <Typography variant="subtitle2" gutterBottom>Image Preview</Typography>
            <CardMedia
              component="img"
              image={value}
              alt="Product Image"
              sx={{ 
                maxHeight: '400px',
                objectFit: 'contain',
                backgroundColor: 'background.paper'
              }}
            />
          </TableCell>
        </TableRow>
      );
    }
    
    // Handle special case for text data
    if (key === 'data' && typeof value === 'string' && product.type === 'Text') {
      return (
        <TableRow key={key}>
          <TableCell component="th" scope="row" colSpan={2}>
            <Typography variant="subtitle2" gutterBottom>Text Content</Typography>
            <Paper 
              variant="outlined" 
              sx={{ 
                p: 2, 
                backgroundColor: 'background.default',
                fontFamily: 'monospace',
                whiteSpace: 'pre-wrap',
                overflowWrap: 'break-word'
              }}
            >
              {value}
            </Paper>
          </TableCell>
        </TableRow>
      );
    }
    
    // For all other fields, render as a standard table row
    return (
      <TableRow key={key}>
        <TableCell component="th" scope="row">
          {/* Convert key to title case for display */}
          {key.split('_').map(word => word.charAt(0).toUpperCase() + word.slice(1)).join(' ')}
        </TableCell>
        <TableCell sx={{ 
          wordBreak: 'break-all',
          fontFamily: typeof value === 'number' || key.includes('hex') || key.includes('binary') ? 'monospace' : 'inherit'
        }}>
          {typeof value === 'object' ? JSON.stringify(value) : String(value)}
        </TableCell>
      </TableRow>
    );
  };

  return (
    <Box sx={{ width: '100%' }}>
      <Typography variant="subtitle2" color="text.secondary" gutterBottom>
        ID: {product.id}
      </Typography>
      
      <Card>
        <CardContent>
          <TableContainer component={Paper} variant="outlined">
            <Table>
              <TableBody>
                {/* Always show type first */}
                <TableRow>
                  <TableCell component="th" scope="row">Type</TableCell>
                  <TableCell>{product.formatted_type || product.type}</TableCell>
                </TableRow>
                
                {/* Render all other fields */}
                {Object.entries(product).map(([key, value]) => 
                  renderField(key, value)
                )}
              </TableBody>
            </Table>
          </TableContainer>
        </CardContent>
      </Card>
    </Box>
  );
}
